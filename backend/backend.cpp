#include <iostream>
#include <string>
#include <sqlite3.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

class BusReservation {
private:
    sqlite3* db;

public:
    BusReservation() {
        sqlite3_open("../database/bus_reservation.db", &db);

        string sql =
            "CREATE TABLE IF NOT EXISTS buses("
            "id INTEGER PRIMARY KEY,"
            "name TEXT,"
            "source TEXT,"
            "destination TEXT,"
            "seats INTEGER);"

            "CREATE TABLE IF NOT EXISTS bookings("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "passenger TEXT,"
            "bus_id INTEGER,"
            "seat INTEGER,"
            "UNIQUE(bus_id, seat));";

        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

        // Add sample buses only if table is empty
        sqlite3_stmt* stmt;

        sqlite3_prepare_v2(
            db,
            "SELECT COUNT(*) FROM buses;",
            -1,
            &stmt,
            nullptr
        );

        int count = 0;

        if (sqlite3_step(stmt) == SQLITE_ROW)
            count = sqlite3_column_int(stmt, 0);

        sqlite3_finalize(stmt);

            string buses =
                "INSERT OR Ingore INTO buses VALUES"
                "(1,'Bhopal Express','Bhopal','Indore',40),"
                "(2,'City Rider','Bhopal','Jabalpur',40),"
                "(3,'MP Express','Indore','Bhopal',40),"
                "(4, 'Indore Express', 'Indore', 'Bhopal', 40),"
                "(5, 'Ind-Ujjain Express', 'Indore', 'Ujjain', 40),"
                "(6, 'Ind-Jabalpur', 'Indore', 'Jabalpur', 40),"
                "(7, 'Capital', 'Jabalpur', 'Bhopal', 40),"
                "(8, 'Jabalpur-Ind', 'Jabalpur', 'Indore', 40),"
                "(9, 'Jabalpur-Mahakal', 'Jabalpur', 'Ujjain', 40),"
                "(10, 'City Express', 'Ujjain', 'Bhopal', 40)"
                "(11, 'Ujj-Ind', 'Ujjain', 'Indore', 40)"
                "(12, 'Ujj-Jab', 'Ujjain', 'Jabalpur', 40)";

            sqlite3_exec(
                db,
                buses.c_str(),
                nullptr,
                nullptr,
                nullptr
            );
    }

    ~BusReservation() {
        sqlite3_close(db);
    }

    string showBuses() {
        sqlite3_stmt* stmt;

        sqlite3_prepare_v2(
            db,
            "SELECT * FROM buses;",
            -1,
            &stmt,
            nullptr
        );

        string result = "";

        while (sqlite3_step(stmt) == SQLITE_ROW) {

            int id = sqlite3_column_int(stmt, 0);

            string name =
                (char*)sqlite3_column_text(stmt, 1);

            string source =
                (char*)sqlite3_column_text(stmt, 2);

            string destination =
                (char*)sqlite3_column_text(stmt, 3);

            int seats =
                sqlite3_column_int(stmt, 4);

            result +=
                to_string(id) + "|" +
                name + "|" +
                source + "|" +
                destination + "|" +
                to_string(seats) + "\n";
        }

        sqlite3_finalize(stmt);

        return result;
    }

    string bookTicket(
        string passenger,
        int busId,
        int seat
    ) {
        string sql =
            "INSERT INTO bookings"
            "(passenger,bus_id,seat)"
            "VALUES(?,?,?);";

        sqlite3_stmt* stmt;

        sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr
        );

        sqlite3_bind_text(
            stmt,
            1,
            passenger.c_str(),
            -1,
            SQLITE_TRANSIENT
        );

        sqlite3_bind_int(stmt, 2, busId);
        sqlite3_bind_int(stmt, 3, seat);

        int result = sqlite3_step(stmt);

        sqlite3_finalize(stmt);

        if (result == SQLITE_DONE)
            return "Ticket booked successfully!";

        return "Seat already booked!";
    }

    string cancelTicket(int bookingId) {

        string sql =
            "DELETE FROM bookings WHERE id=?;";

        sqlite3_stmt* stmt;

        sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr
        );

        sqlite3_bind_int(
            stmt,
            1,
            bookingId
        );

        sqlite3_step(stmt);

        int changes = sqlite3_changes(db);

        sqlite3_finalize(stmt);

        if (changes > 0)
            return "Ticket cancelled successfully!";

        return "Booking not found!";
    }
};


// --------------------------------------------------
// SIMPLE HTTP SERVER
// --------------------------------------------------

string response(string data) {

    string r =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Content-Length: " +
        to_string(data.size()) +
        "\r\n\r\n";

    return r + data;
}


string getValue(string body, string key) {

    string search = key + "=";

    size_t start = body.find(search);

    if (start == string::npos)
        return "";

    start += search.length();

    size_t end = body.find("&", start);

    if (end == string::npos)
        end = body.length();

    return body.substr(
        start,
        end - start
    );
}


int main() {

    BusReservation system;

    int server =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(18080);

    bind(
        server,
        (struct sockaddr*)&address,
        sizeof(address)
    );

    listen(server, 10);

    cout << "================================\n";
    cout << "Bus Reservation Server Started\n";
    cout << "http://localhost:18080\n";
    cout << "================================\n";

    while (true) {

        int client = accept(
            server,
            nullptr,
            nullptr
        );

        char buffer[8192] = {};

        int size = recv(
            client,
            buffer,
            sizeof(buffer),
            0
        );

        string request(buffer, size);

        string result;

        // GET BUSES
        if (request.find("GET /buses") == 0) {

            result =
                system.showBuses();
        }

        // BOOK TICKET
        else if (
            request.find("POST /book") == 0
        ) {

            size_t pos =
                request.find("\r\n\r\n");

            string body =
                request.substr(pos + 4);

            string passenger =
                getValue(body, "passenger");

            int busId =
                stoi(getValue(body, "bus"));

            int seat =
                stoi(getValue(body, "seat"));

            result =
                system.bookTicket(
                    passenger,
                    busId,
                    seat
                );
        }

        // CANCEL TICKET
        else if (
            request.find("POST /cancel") == 0
        ) {

            size_t pos =
                request.find("\r\n\r\n");

            string body =
                request.substr(pos + 4);

            int bookingId =
                stoi(getValue(body, "id"));

            result =
                system.cancelTicket(
                    bookingId
                );
        }

        else {

            result = "Invalid request";
        }

        string r = response(result);

        send(
            client,
            r.c_str(),
            r.size(),
            0
        );

        close(client);
    }

    return 0;
}
