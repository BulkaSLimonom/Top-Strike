#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <cmath>

using namespace sf;




char menu(const int WINDOW_WIDTH, const int WINDOW_HEIGHT) {
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Top strike", Style::Default);

    window.setVerticalSyncEnabled(true);

    RectangleShape button_play(Vector2f(200, 35));
    RectangleShape button_exit(Vector2f(200, 35));
    button_play.setFillColor(Color(0, 150, 0));
    button_exit.setFillColor(Color(0, 150, 0));
    Font font;
    font.loadFromFile("arialmt.ttf");
    Text play_text;
    play_text.setString("Play");
    play_text.setCharacterSize(30);
    play_text.setFillColor(Color::White);
    play_text.setFont(font);
    Text exit_text;
    exit_text.setString("Exit");
    exit_text.setCharacterSize(30);
    exit_text.setFillColor(Color::White);
    exit_text.setFont(font);
    bool play_button_pressed = false;

    RectangleShape button_server(Vector2f(150, 50));
    RectangleShape button_client(Vector2f(150, 50));
    button_server.setFillColor(Color(0, 150, 0));
    button_client.setFillColor(Color(0, 150, 0));
    Text server_text("Server", font, 30);
    Text client_text("Client", font, 30);
    server_text.setFillColor(Color::White);
    client_text.setFillColor(Color::White);
    bool server_or_client_button_pressed = true;

    char mode = 'r';

    while (window.isOpen())
    {
        Vector2i mousePoz = Mouse::getPosition(window);

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            else if (event.type == Event::Resized) {
                window.setSize(Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT));
            }
            if (!play_button_pressed) {
                if (event.type == Event::MouseButtonReleased) {
                    if (event.key.code == Mouse::Left) {
                        if (button_play.getGlobalBounds().contains(mousePoz.x, mousePoz.y)) {
                            //std::cout << "pressed" << std::endl;
                            play_button_pressed = true;
                            server_or_client_button_pressed = false;
                        }
                        if (button_exit.getGlobalBounds().contains(mousePoz.x, mousePoz.y)) {
                            window.close();
                        }
                    }
                }
            }
            if (!server_or_client_button_pressed) {
                if (event.type == Event::MouseButtonReleased) {
                    if (event.key.code == Mouse::Left) {
                        if (button_server.getGlobalBounds().contains(mousePoz.x, mousePoz.y)) {
                            server_or_client_button_pressed = true;
                            //std::cout << "server button pressed" << std::endl;
                            mode = 's';
                            window.close();
                        }
                        if (button_client.getGlobalBounds().contains(mousePoz.x, mousePoz.y)) {
                            server_or_client_button_pressed = true;
                            //std::cout << "client button pressed" << std::endl;
                            mode = 'c';
                            window.close();
                        }
                    }
                }
            }

        }

        window.clear();



        button_play.setPosition({ 45, 100 });
        button_exit.setPosition({ 45, 175 });
        play_text.setPosition({ 60, 100 });
        exit_text.setPosition({ 60, 175 });

        button_server.setPosition({ 400, 300 });
        button_client.setPosition({ 600, 300 });
        server_text.setPosition({ 415, 300 });
        client_text.setPosition({ 615, 300 });

        if (!play_button_pressed) {
            window.draw(button_play);
            window.draw(button_exit);
            window.draw(play_text);
            window.draw(exit_text);
        }
        if (!server_or_client_button_pressed) {
            window.draw(button_server);
            window.draw(button_client);
            window.draw(server_text);
            window.draw(client_text);
        }

        window.display();
    }
    return(mode);
}



















int main()
{
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const float player_speed = 0.08;
    const float bullet_speed = 1.5;
    const double pi = 3.14159265358979323846;
    //int CT_spawn_x = 0;
    //int CT_spawn_y = 0;
    //int T_spawn_x = 0;
    //int T_spawn_y = 0;
    Vector2f CTpos(0, 0);
    Vector2f Tpos(0, 0);

    float distance = 0;
    //Vector2f temp_bullet(0, 0);


    char mode = menu(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (mode == 's') {
        CTpos = Vector2f(100, WINDOW_HEIGHT / 2);
        Tpos = Vector2f(WINDOW_WIDTH - 100, WINDOW_HEIGHT / 2);
    }
    else if (mode == 'c') {
        Tpos = Vector2f(100, WINDOW_HEIGHT / 2);
        CTpos = Vector2f(WINDOW_WIDTH - 100, WINDOW_HEIGHT / 2);
    }
    else {
        return 0;
    }

    //std::cout << mode << std::endl;

    

    IpAddress ip = IpAddress::getLocalAddress();
    TcpSocket socket;
    //Packet packet;
    char buffer[2000];
    size_t received;
    std::string text = "connect to: ";

    if (mode == 's') {
        TcpListener listener;
        listener.listen(2000);
        listener.accept(socket);
        text += "server";
    }
    else if (mode == 'c') {
        std::cout << "enter your ip address to connect (left empty for local ip address): ";
        std::cin >> std::noskipws >> ip;
        if (ip == "\n") {
            ip = IpAddress::getLocalAddress();
        }
        socket.connect(ip, 2000);
        text += "client";

    }

    socket.send(text.c_str(), text.length() + 1);
    socket.receive(buffer, sizeof(buffer), received);
    std::cout << buffer << ", ip: " << ip << std::endl;

    CircleShape CT(10);
    CT.setOrigin(CT.getRadius(), CT.getRadius());
    //Vector2f CTpos(100, WINDOW_HEIGHT / 2);
    bool CTlife = true;
    CT.setFillColor(Color::Cyan);

    CircleShape T(10);
    T.setOrigin(T.getRadius(), T.getRadius());
    //Vector2f Tpos(WINDOW_WIDTH - 100, WINDOW_HEIGHT / 2);
    bool Tlife = true;
    T.setFillColor(Color::Red);

    CircleShape bulletCT(2);
    bulletCT.setOrigin(bulletCT.getRadius(), bulletCT.getRadius());
    Vector2f bullet_coordsCT(CTpos.x, CTpos.y);
    bulletCT.setFillColor(Color::Cyan);

    CircleShape bulletT(2);
    bulletT.setOrigin(bulletT.getRadius(), bulletT.getRadius());
    Vector2f bullet_coordsT(Tpos.x, Tpos.y);
    bulletT.setFillColor(Color::Red);

    RectangleShape SpawnRectCT(Vector2f(100, 300));
    RectangleShape SpawnRectT(Vector2f(100, 300));
    SpawnRectCT.setPosition(300, WINDOW_HEIGHT / 2 - 150);
    SpawnRectT.setPosition(WINDOW_WIDTH - 300, WINDOW_HEIGHT / 2 - 150);

    RenderWindow window_game(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32), "Game", Style::Default);

    socket.setBlocking(false);

    Vector2f prevPosition;

    Clock clock;
    Clock winsleep;

    bool Sleep = false;
    float elapsed = 0.0f;

    bool update = true;

    bool isDrawWin = false;
    bool isDrawLose = false;

    float ReloadTime = 0.0f;

    bool miss = true;

    //Vector2i temp_bullet;

    //bool temp_life = false;

    Vector2f velocity;

    /*RectangleShape aim(Vector2f(WINDOW_WIDTH, 1));
    aim.setFillColor(Color(255, 0, 0));
    aim.setPosition(CT.getPosition().x + 10, CT.getPosition().y + 9.75);

    CircleShape bullet(1);
    Vector2f bullet_coords = (CT.getPosition());
    bullet.setPosition(bullet_coords);*/

    Font font;
    font.loadFromFile("arialmt.ttf");

    int scoreCT = 0;
    int scoreT = 0;

    Text RoundCT(std::to_string(scoreCT), font);
    Text RoundT(std::to_string(scoreT), font);
    Text RoundWin("Round win", font);
    Text RoundLose("Round lose", font);
    RoundWin.setCharacterSize(35);
    RoundWin.setPosition(200, 100);
    RoundLose.setCharacterSize(35);
    RoundLose.setPosition(200, 100);

    RoundCT.setCharacterSize(10);
    RoundT.setCharacterSize(10);
    RoundCT.setPosition((WINDOW_WIDTH / 2) - 100, WINDOW_HEIGHT - 700);
    RoundT.setPosition((WINDOW_WIDTH / 2) + 100, WINDOW_HEIGHT - 700);


    CT.setPosition(CTpos);
    T.setPosition(Tpos);
    //float time2 = 0;
    //float counter_aim = 501;

    while (window_game.isOpen()) {

        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        time /= 300;
        //std::cout << time << std::endl;
        ReloadTime += time;

        Event event;
        while (window_game.pollEvent(event))
        {
            if (event.type == Event::Closed) {
                window_game.close();
            }
            else if (event.type == Event::Resized) {
                window_game.setSize(Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT));
            }
            else if (event.type == event.GainedFocus) {
                update = true;
            }
            else if (event.type == event.LostFocus) {
                update = false;
            }
            if (!Sleep) {
                if (event.type == Event::MouseButtonReleased and ReloadTime >= 5000) {
                    if (event.key.code == Mouse::Left) {
                        if (miss) {
                            Vector2i temp_bullet = Mouse::getPosition(window_game);
                            //CTposfriz = CTpos;
                            //distance = sqrt((temp_bullet.x - bullet_coords.x) * (temp_bullet.x - bullet_coords.x) + (temp_bullet.y - bullet_coords.y) * (temp_bullet.y - bullet_coords.y));
                            Vector2f direction = Vector2f(temp_bullet) - Vector2f(CTpos.x + 4.5, CTpos.y + 4.5);
                            float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
                            direction.x /= magnitude;
                            direction.y /= magnitude;

                            velocity = direction * bullet_speed;
                            miss = false;
                            ReloadTime = 0.0f;
                        }
                    }
                }
            }
            //if (event.type == event.MouseButtonReleased) {
            //    if (event.key.code == Mouse::Left) {
            //        //if (aim.getGlobalBounds().intersects(T.getGlobalBounds())) {
            //        //    std::cout << "you died" << std::endl;
            //        //}
            //        miss = false;
            //        temp_bullet = Vector2f(Mouse::getPosition(window_game));
            //    }
            //}
            //if (!(T.getGlobalBounds().contains(bullet.getPosition().x, bullet.getPosition().y))) {
            //    if (bullet_coords.x == WINDOW_WIDTH or bullet_coords.x == 0 or bullet_coords.y == WINDOW_HEIGHT or bullet_coords.y == 0) {
            //        miss = true;
            //    }
            //}
            //else {
            //    std::cout << "you died" << std::endl;
            //    miss = true;
            //}
            //if (miss) {
            //    bullet_coords = CT.getPosition();
            //}
            //else {
            //    distance = sqrt((temp_bullet.x - p.x) * (tempX - p.x) + (tempY - p.y) * (tempY - p.y));//считаем дистанцию (длину от точки ј до точки Ѕ). формула длины вектора
            //    p.x += 0.1 * time * (tempX - p.x) / distance;//идем по иксу с помощью вектора нормали
            //    p.y += 0.1 * time * (tempY - p.y) / distance;//идем по игреку так же
            //}
                
        }
        if (Sleep) {
            if (elapsed = winsleep.getElapsedTime().asSeconds() >= 3.0f) {
                Sleep = false;
                isDrawLose = false;
                isDrawWin = false;
            }
        }
        if (!Sleep) {
            if (scoreCT >= 3) {
                window_game.close();
            }
            if (scoreT >= 3) {
                window_game.close();
            }
            if (miss) {
                bullet_coordsCT = Vector2f(CTpos.x, CTpos.y);
            }
            if (bullet_coordsCT.x >= 0 and bullet_coordsCT.x <= WINDOW_WIDTH and bullet_coordsCT.y >= 0 and bullet_coordsCT.y <= WINDOW_HEIGHT) {
                if (!miss) {

                    //distance = sqrt((temp_bullet.x - bullet_coords.x) * (temp_bullet.x - bullet_coords.x) + (temp_bullet.y - bullet_coords.y) * (temp_bullet.y - bullet_coords.y));
                    //distance = sqrt((temp_bullet.x - bullet_coords.x - WINDOW_WIDTH) * (temp_bullet.x - bullet_coords.x - WINDOW_WIDTH) + (temp_bullet.y - bullet_coords.y - WINDOW_HEIGHT) * (temp_bullet.y - bullet_coords.y - WINDOW_HEIGHT));
                    //std::cout << distance << std::endl;
                    //bullet_coords.x += bullet_speed * (temp_bullet.x - CTposfriz.x) / distance;
                    //bullet_coords.y += bullet_speed * (temp_bullet.y - CTposfriz.y) / distance;
                    Vector2f bullet_coords_new = bullet_coordsCT;
                    bullet_coords_new += velocity * time;
                    if (!SpawnRectCT.getGlobalBounds().contains(bullet_coords_new) and !SpawnRectT.getGlobalBounds().contains(bullet_coords_new)) {
                        bullet_coordsCT += velocity * time;
                    }
                    else { miss = true; }
                }
            }
            else {
                miss = true;
            }
            if (T.getGlobalBounds().contains(bullet_coordsCT)) {
                miss = true;
                Tlife = false;
                scoreCT++;
            }
            prevPosition = CT.getPosition();
            //float time2 = clock2.getElapsedTime().asMilliseconds();
            //counter_aim += time2;
            //clock2.restart();

            if (update) {

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) and (CTpos.x + CT.getRadius()) < WINDOW_WIDTH) {
                    Vector2f CTposnew = CTpos;
                    CTposnew.x += player_speed * time + CT.getRadius();
                    CTposnew.y += CT.getRadius();
                    if (!SpawnRectCT.getGlobalBounds().contains(CTposnew) and !SpawnRectT.getGlobalBounds().contains(CTposnew) and !T.getGlobalBounds().contains(CTposnew)) {
                        CTposnew.y -= 2 * CT.getRadius();
                        if (!SpawnRectCT.getGlobalBounds().contains(CTposnew) and !SpawnRectT.getGlobalBounds().contains(CTposnew) and !T.getGlobalBounds().contains(CTposnew))
                            CTpos.x += player_speed * time;
                    }
                    //std::cout << "work" << std::endl;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) and (CTpos.x - CT.getRadius()) > 0) {
                    //std::cout << "work" << std::endl;
                    Vector2f CTposnew = CTpos;
                    CTposnew.x -= player_speed * time + CT.getRadius();
                    CTposnew.y += CT.getRadius();
                    if (!SpawnRectCT.getGlobalBounds().contains(CTposnew) and !SpawnRectT.getGlobalBounds().contains(CTposnew) and !T.getGlobalBounds().contains(CTposnew)) {
                        CTposnew.y -= 2 * CT.getRadius();
                        if (!SpawnRectCT.getGlobalBounds().contains(CTposnew) and !SpawnRectT.getGlobalBounds().contains(CTposnew) and !T.getGlobalBounds().contains(CTposnew))
                            CTpos.x -= player_speed * time;
                    }
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) and (CTpos.y - CT.getRadius()) > 0) {
                    Vector2f CTposnew = CTpos;
                    //std::cout << "work" << std::endl;
                    CTposnew.y -= player_speed * time + CT.getRadius();
                    CTposnew.x += CT.getRadius();
                    if (!SpawnRectCT.getGlobalBounds().contains(CTposnew) and !SpawnRectT.getGlobalBounds().contains(CTposnew) and !T.getGlobalBounds().contains(CTposnew)) {
                        CTposnew.x -= 2 * CT.getRadius();
                        if (!SpawnRectCT.getGlobalBounds().contains(CTposnew) and !SpawnRectT.getGlobalBounds().contains(CTposnew) and !T.getGlobalBounds().contains(CTposnew))
                            CTpos.y -= player_speed * time;
                    }
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) and (CTpos.y + CT.getRadius()) < WINDOW_HEIGHT) {
                    Vector2f CTposnew = CTpos;
                    //std::cout << "work" << std::endl;
                    CTposnew.y += player_speed * time + CT.getRadius();
                    CTposnew.x += CT.getRadius();
                    if (!SpawnRectCT.getGlobalBounds().contains(CTposnew) and !SpawnRectT.getGlobalBounds().contains(CTposnew) and !T.getGlobalBounds().contains(CTposnew)) {
                        CTposnew.x -= 2 * CT.getRadius();
                        if (!SpawnRectCT.getGlobalBounds().contains(CTposnew) and !SpawnRectT.getGlobalBounds().contains(CTposnew) and !T.getGlobalBounds().contains(CTposnew))
                            CTpos.y += player_speed * time;
                    }
                }
            }
            // if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
             //    std::cout << "lmb pressed" << std::endl;

                 //std::cout << "tangens - " << tangens_alpha << std::endl << "alpha - " << alpha << std::endl << "coordinates of aim - " << aim.getPosition().x << ":" << aim.getPosition().y << std::endl;


                 //counter_aim = 0;
             //}
        }
        CT.setPosition(CTpos);
        bulletCT.setPosition(bullet_coordsCT);
        Packet packetpos;
        if (prevPosition != CT.getPosition() or !miss or !Tlife) {
            packetpos << CT.getPosition().x << CT.getPosition().y << bulletCT.getPosition().x << bulletCT.getPosition().y << Tlife;
            socket.send(packetpos);
        }
        socket.receive(packetpos);
        if (packetpos >> Tpos.x >> Tpos.y >> bullet_coordsT.x >> bullet_coordsT.y >> CTlife) {
            T.setPosition(Tpos);
            bulletT.setPosition(bullet_coordsT);
        }
        if (!CTlife) {
            scoreT++;
        }
        //CT.setPosition(CTpos);
        //bulletCT.setPosition(bullet_coordsCT);
        //Packet packetpos;
        //Packet packetbullet;
        //Packet packetlife;
        //if (prevPosition != CT.getPosition()) {
        //    packetpos << CT.getPosition().x << CT.getPosition().y;
        //    //packet << CT.getPosition().x << CT.getPosition().y << CTlife;
        //    socket.send(packetpos);
        //    //std::cout << CT.getPosition().x << ":" << CT.getPosition().y << std::endl;
        //}
        //socket.receive(packetpos);

        //if (packetpos >> Tpos.x >> Tpos.y) {
        //    T.setPosition(Tpos);
        //    
        //}
        ////packet.clear();
        //if (!miss) {
        //    packetbullet << bulletCT.getPosition().x << bulletCT.getPosition().y;
        //    socket.send(packetbullet);
        //}
        //socket.receive(packetbullet);
        //
        //if (packetbullet >> bullet_coordsT.x >> bullet_coordsT.y) {
        //    bulletT.setPosition(bullet_coordsT);
        //}
        ////packet.clear();

        //if (!Tlife) {
        //    packetlife << Tlife;
        //    socket.send(packetlife);
        //}
        //socket.receive(packetlife);
        //packetlife >> CTlife;
        /*if (packet >> Tpos.x >> Tpos.y >> Tlife) {
            T.setPosition(Tpos);
        }*/
        
            //packet.clear();

            //sf::Cursor cursor;
            //cursor.loadFromSystem(sf::Cursor::Cross);
            //window_game.setMouseCursor(cursor);

        //CT.setPosition(CT_v);
        //T.setPosition(T_v);

        /*if (update) {
            Vector2f tmp_lmb_to_player;
            tmp_lmb_to_player.x = Mouse::getPosition(window_game).x - CT.getPosition().x;
            tmp_lmb_to_player.y = Mouse::getPosition(window_game).y - CT.getPosition().y;
            float tangens_alpha = tmp_lmb_to_player.y / tmp_lmb_to_player.x;
            float alpha = atan2(tmp_lmb_to_player.y, tmp_lmb_to_player.x) * (180.0 / pi);
            aim.setRotation(alpha);
            aim.setPosition(CT.getPosition().x + 10, CT.getPosition().y + 9.75);
        }*/
        

        /*window_game.clear(Color::Black);
        window_game.draw(aim);
        window_game.draw(CT);
        window_game.draw(T);
        window_game.display();*/
        /*if (mode == 'c') {
            sleep(milliseconds(20));
        }*/
        
        //T.setPosition(Tpos);
        bulletCT.setPosition(bullet_coordsCT);

        window_game.clear(Color::Black);
        if (!miss) {
            window_game.draw(bulletCT);
        }
        if (CTlife) {
            window_game.draw(CT);
        }
        else {
            Sleep = true;
            winsleep.restart();
            if (mode == 's') {
                CTpos = Vector2f(100, WINDOW_HEIGHT / 2);
                Tpos = Vector2f(WINDOW_WIDTH - 100, WINDOW_HEIGHT / 2);
            }
            else if (mode == 'c') {
                Tpos = Vector2f(100, WINDOW_HEIGHT / 2);
                CTpos = Vector2f(WINDOW_WIDTH - 100, WINDOW_HEIGHT / 2);
            }
            CTlife = true;
            isDrawLose = true;
            bullet_coordsCT = CTpos;
            bullet_coordsT = Tpos;
            bulletT.setPosition(bullet_coordsT);
            bulletCT.setPosition(bullet_coordsCT);
        }
        if (Tlife) {
            window_game.draw(T);
        }
        else {
            //if (temp_life) {
            //    temp_life = false;
            //    //sleep(seconds(3));
            //    CTpos = Vector2f(100, WINDOW_HEIGHT / 2);
            //    Tpos = Vector2f(WINDOW_WIDTH - 100, WINDOW_HEIGHT / 2);
            //    Tlife = true;
            //    
            //}
            //else {
            //    temp_life = true;
            //}
            Sleep = true;
            winsleep.restart();
            if (mode == 's') {
                CTpos = Vector2f(100, WINDOW_HEIGHT / 2);
                Tpos = Vector2f(WINDOW_WIDTH - 100, WINDOW_HEIGHT / 2);
            }
            else if (mode == 'c') {
                Tpos = Vector2f(100, WINDOW_HEIGHT / 2);
                CTpos = Vector2f(WINDOW_WIDTH - 100, WINDOW_HEIGHT / 2);
            }
            Tlife = true;
            isDrawWin = true;
            bullet_coordsCT = CTpos;
            bullet_coordsT = Tpos;
            bulletT.setPosition(bullet_coordsT);
            bulletCT.setPosition(bullet_coordsCT);

        }
        if (isDrawWin) {
            window_game.draw(RoundWin);
        }
        if (isDrawLose) {
            window_game.draw(RoundLose);
        }
        RoundCT.setString(std::to_string(scoreCT));
        RoundT.setString(std::to_string(scoreT));
        window_game.draw(RoundCT);
        window_game.draw(RoundT);
        window_game.draw(SpawnRectCT);
        window_game.draw(SpawnRectT);
        window_game.draw(bulletT);


        window_game.display();
        /*if (mode == 'c') {
            sleep(microseconds(20));
        }*/
    }
}