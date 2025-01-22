#include <iostream>
#include <SDL.h>
#include<stack>
#include <SDL_ttf.h>
#include<stdexcept>
#include<conio.h>
#include <unordered_map>
#include <vector>
#include <queue>
#include <climits>
#include "./Color.hpp"
#include <algorithm>
#include <windows.h>
using namespace std;
void color(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void gotoxy(int x, int y)
{
    COORD c;
    c.X = x;
    c.Y = y;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
TTF_Font* gFont = nullptr;
SDL_Texture* gMapTexture = nullptr;

class VisualizerCities
{
public:
    SDL_Window* gWindow = nullptr;
    SDL_Renderer* gRenderer = nullptr;
    TTF_Font* gFont = nullptr;

    void closeSDL()
    {
        SDL_DestroyTexture(gMapTexture);
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        TTF_CloseFont(gFont);
        TTF_Quit();
        SDL_Quit();
    }

    bool initSDL()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            cerr << "SDL initialization failed. SDL Error: " << SDL_GetError() << endl;
            return false;
        }

        gWindow = SDL_CreateWindow("City Distance Visualizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 1080, SDL_WINDOW_SHOWN);
        if (gWindow == nullptr)
        {
            cerr << "Window creation failed. SDL Error: " << SDL_GetError() << endl;
            return false;
        }

        gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (gRenderer == nullptr)
        {
            cerr << "Renderer creation failed. SDL Error: " << SDL_GetError() << endl;
            return false;
        }

        if (TTF_Init() == -1)
        {
            cerr << "SDL_ttf initialization failed. TTF Error: " << TTF_GetError() << endl;
            return false;
        }

        gFont = TTF_OpenFont("arial.ttf", 16); // Use the correct path to your Arial font file
        if (gFont == nullptr)
        {
            cerr << "Failed to load font. TTF Error: " << TTF_GetError() << endl;
            return false;
        }
        TTF_SetFontStyle(gFont, TTF_STYLE_BOLD);
        // Load map texture
        SDL_Surface* mapSurface = SDL_LoadBMP("map_of_india.bmp"); // Replace with the actual file path
        if (mapSurface == nullptr)
        {
            cerr << "Failed to load map texture. SDL Error: " << SDL_GetError() << endl;
            return false;
        }

        gMapTexture = SDL_CreateTextureFromSurface(gRenderer, mapSurface);
        SDL_FreeSurface(mapSurface);

        SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

        return true;
    }

    void drawCity(string cityName, int x, int y)
    {
        SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255); // Red color for points
        int circleRadius = 5; // Adjust the radius as needed
        for (int i = -circleRadius; i <= circleRadius; ++i)
        {
            for (int j = -circleRadius; j <= circleRadius; ++j)
            {
                if (i * i + j * j <= circleRadius * circleRadius)
                {
                    SDL_RenderDrawPoint(gRenderer, x + i, y + j);
                }
            }
        }
        SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

        SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, cityName.c_str(), { 255, 0, 0, 255 });
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);

        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
        int textX = x - textWidth / 2;
        int textY = y + circleRadius + 5;
        SDL_Rect destRect = { textX, textY, textWidth, textHeight };
        SDL_RenderCopy(gRenderer, textTexture, nullptr, &destRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    void drawEdge(int sourceX, int sourceY, int destX, int destY)
    {
        // Draw the edge in blue color
        SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255); // Blue color for edges
        const int thickness = 5; // Adjust the thickness as needed
        for (int i = 0; i < thickness; ++i)
        {
            SDL_RenderDrawLine(gRenderer, sourceX + i, sourceY, destX + i, destY);
        }

        SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
        SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);
    }
    void drawVisitedNode(int x, int y)
    {
        SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255); // Green color for visited nodes
        int circleRadius = 5; // Adjust the radius as needed

        // Draw the circle for visited node
        for (int i = -circleRadius; i <= circleRadius; ++i)
        {
            for (int j = -circleRadius; j <= circleRadius; ++j)
            {
                if (i * i + j * j <= circleRadius * circleRadius)
                {
                    SDL_RenderDrawPoint(gRenderer, x + i, y + j);
                }
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    }


    void renderMap()
    {
        SDL_RenderCopy(gRenderer, gMapTexture, nullptr, nullptr);
    }
};

struct CityInfo
{
    int x;
    int y;
};

unordered_map<string, CityInfo> cityPositions;

unordered_map<string, vector<pair<string, int>>> adjacencyList;

void initializeCities()
{
    // Add positions for 10 famous cities in India
    cityPositions["Delhi"] = { 490, 360 };
    cityPositions["Mumbai"] = { 260, 690 };
    cityPositions["Chennai"] = { 650, 870 };
    cityPositions["Kolkata"] = { 1100, 580 };
    cityPositions["Bangalore"] = { 520, 870 };
    cityPositions["Hyderabad"] = { 550, 740 };
    cityPositions["Ahmedabad"] = { 250, 550 };
    cityPositions["Jaipur"] = { 420, 420 };
    cityPositions["Lucknow"] = { 650, 400 };
    cityPositions["Agra"] = { 550, 400 };
    cityPositions["Guwahati"] = { 1330, 435 };
    cityPositions["Srinagar"] = { 350, 150 };
    cityPositions["Bhubaneshwar"] = { 990, 630 };
    cityPositions["Thiruvananthapuram"] = { 465, 1000 };
}

pair<vector<string>, int> dijkstra(const string& startCity, const string& endCity)
{
    unordered_map<string, int> distance;
    unordered_map<string, string> previous;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;

    // Initialize distances
    for (const auto& city : cityPositions)
    {
        distance[city.first] = INT_MAX;
        previous[city.first] = "";
    }

    distance[startCity] = 0;
    pq.push({ 0, startCity });

    while (!pq.empty())
    {
        int currDistance = pq.top().first;
        string currCity = pq.top().second;
        pq.pop();

        if (currDistance > distance[currCity])
            continue;

        for (const auto& neighbor : adjacencyList[currCity])
        {
            string nextCity = neighbor.first;
            int weight = neighbor.second;

            int newDistance = currDistance + weight;
            if (newDistance < distance[nextCity])
            {
                distance[nextCity] = newDistance;
                previous[nextCity] = currCity;
                pq.push({ newDistance, nextCity });
            }
        }
    }

    // Reconstruct the shortest path
    vector<string> shortestPath;
    for (string city = endCity; !city.empty(); city = previous[city])
    {
        shortestPath.push_back(city);
    }

    reverse(shortestPath.begin(), shortestPath.end());

    return { shortestPath, distance[endCity] };
}

// ... (existing code)

void bfsTraversalWithVisualization(const string& startCity, VisualizerCities& visualizer)
{
    unordered_map<string, bool> visited;
    queue<string> q;

    q.push(startCity);
    visited[startCity] = true;

    while (!q.empty())
    {
        string currentCity = q.front();
        q.pop();

        const CityInfo& cityInfo = cityPositions[currentCity];
        visualizer.drawVisitedNode(cityInfo.x, cityInfo.y);
        SDL_RenderPresent(visualizer.gRenderer);

        for (const auto& neighbor : adjacencyList[currentCity])
        {
            string nextCity = neighbor.first;
            if (!visited[nextCity])
            {
                q.push(nextCity);
                visited[nextCity] = true;

                const CityInfo& nextCityInfo = cityPositions[nextCity];
                visualizer.drawEdge(cityInfo.x, cityInfo.y, nextCityInfo.x, nextCityInfo.y);

                // Delay for visualization

            }
        }
        SDL_Delay(1000);

    }
}
void dfsTraversalWithVisualization(const string& startCity, VisualizerCities& visualizer)
{
    unordered_map<string, bool> visited;
    stack<string> s;

    s.push(startCity);

    while (!s.empty())
    {
        string currentCity = s.top();
        s.pop();

        if (!visited[currentCity])
        {
            const CityInfo& cityInfo = cityPositions[currentCity];
            visualizer.drawVisitedNode(cityInfo.x, cityInfo.y);
            SDL_RenderPresent(visualizer.gRenderer);

            visited[currentCity] = true;

            for (const auto& neighbor : adjacencyList[currentCity])
            {

                string nextCity = neighbor.first;
                if (!visited[nextCity])
                {
                    s.push(nextCity);

                    const CityInfo& nextCityInfo = cityPositions[nextCity];
                    visualizer.drawEdge(cityInfo.x, cityInfo.y, nextCityInfo.x, nextCityInfo.y);
                }
            }

        }
        SDL_Delay(1000);
    }
}
string city(int i)
{
    switch (i)
    {
    case 1:
        return "Delhi";
        break;
    case 2:
        return "Mumbai";
        break;
    case 3:
        return "Chennai";
        break;
    case 4:
        return "Kolkata";
        break;
    case 5:
        return "Bangalore";
        break;
    case 6:
        return "Hyderabad";
        break;
    case 7:
        return "Ahmedabad";
        break;
    case 8:
        return "Jaipur";
        break;
    case 9:
        return "Lucknow";
        break;
    case 10:
        return "Srinagar";
        break;
    case 11:
        return "Bhubaneshwar";
        break;
    case 12:
        return "Thiruvananthapuram";
        break;
    case 13:
        return "Agra";
        break;
    case 14:
        return "Guwahati";
        break;
    default:
        return "";
    }
}

int main(int argc, char* args[])
{

menu:
    system("cls");
    int Set[4];
    for (int j = 0; j < 4; j++) {
        Set[j] = 14;
    }
    int counter = 1;
    char key;
    int i = 0;
    cout <<dye::red("                           ....................WELCOME TO TRAVEL MATE.................. ") << endl<<endl<<endl;
    while (1) {

        gotoxy(7, 8);
        color(Set[0]);
        cout << ("1. View map") << endl;
        gotoxy(7, 9);
        color(Set[1]);
        cout << ("2. Travel!") << endl;
        gotoxy(7, 10);
        color(Set[2]);
        cout << "3. India Tour! (Using breadth first search(bfs))" << endl;
        gotoxy(7, 11);
        color(Set[3]);
        cout << "4. India Tour! (Using deapth first search(dfs))" << endl;
        key = _getch();
        if (key == 72 && (counter > 1 && counter <= 7)) {
            counter--;
            i--;
        }

        if (key == 80 && (counter >= 1 && counter < 7)) {
            counter++;
            i++;
        }

        for (int j = 0; j < 4; j++) {
            Set[j] = 14;
        }

        Set[i] = 12;


        if (key == '\r') {
            if(counter==1){
                VisualizerCities visualizer;
                if (!visualizer.initSDL())
                {
                    cout << "SDL initialization failed. Exiting..." << endl;
                    return 1;
                }
                initializeCities();

                // Render the map and show available cities
                visualizer.renderMap();
                for (const auto& city : cityPositions)
                {
                    visualizer.drawCity(city.first, city.second.x, city.second.y);
                }

                // Update the window to show the map and available cities
                SDL_RenderPresent(visualizer.gRenderer);
                bool quit = false;
                SDL_Event e;
                while (!quit)
                {
                    while (SDL_PollEvent(&e) != 0)
                    {
                        if (e.type == SDL_QUIT)
                        {
                            quit = true;
                        }
                        else if (e.type == SDL_KEYDOWN)
                        {
                            quit = true;
                        }
                    }
                }
                visualizer.closeSDL();
                goto menu;
            }
            if (counter == 2) {
                system("cls");
                cout << dye::red("List of cities: ") << endl;
                cout << dye::green("1. Delhi") << endl;
                cout << dye::green("2. Mumbai") << endl;
                cout << dye::green("3. Chennai") << endl;
                cout << dye::green("4. Kolkata") << endl;
                cout << dye::green("5. Bangalore") << endl;
                cout << dye::green("6. Hyderabad") << endl;
                cout << dye::green("7. Ahmedabad") << endl;
                cout << dye::green("8. Jaipur") << endl;
                cout << dye::green("9. Lucknow") << endl;
                cout << dye::green("10. Srinagar") << endl;
                cout << dye::green("11. Bhubaneshwar") << endl;
                cout << dye::green("12. Thiruvananthapuram") << endl;
                cout << dye::green("13. Agra") << endl;
                cout << dye::green("14. Guwahati") << endl << endl;


                string startCity, endCity;
                int i,j;
                int c = 0;
                do  {
                    cout << dye::yellow("Enter the city you want to start from: ");

                    try {
                        cin >> i;
                        if (cin.fail())
                        {
                            throw invalid_argument("Error! Enter a valid number");
                        }
                        cout << dye::yellow("Enter the city you want to go to: ");

                        cin >> j;
                        if (cin.fail())
                        {
                            throw invalid_argument("Error! Enter a valid number");
                        }
                        if (i > 14 || i < 0 || j>14 || j < 0) {
                            throw invalid_argument("Error! Enter a valid number");

                        }
                        c = 0;
                    }
                    catch (const invalid_argument& e)
                    {
                        cerr << e.what() << endl;
                        cin.clear();  // Clear the error flag  // Discard invalid input
                        c = 1;
                    }
                } while (c);
                startCity = city(i);
                endCity = city(j);
                VisualizerCities visualizer;
                if (!visualizer.initSDL())
                {
                    cerr << "SDL initialization failed. Exiting..." << endl;
                    return 1;
                }
                // Initialize city positions
                initializeCities();

                // Initialize adjacency list based on your city positions and connections
                adjacencyList["Delhi"] = { {"Srinagar", 643}, {"Ahmedabad", 775}, {"Jaipur", 241}, {"Lucknow",419 }, {"Agra",180 }, {"Kolkata",1305 } , {"Mumbai",1148 } };
                adjacencyList["Mumbai"] = { {"Bangalore", 987}, {"Ahmedabad", 525},{"Hyderabad", 617}, {"Delhi",1148 } };
                adjacencyList["Chennai"] = { {"Bangalore", 284},{"Hyderabad", 521} };
                adjacencyList["Kolkata"] = { {"Lucknow", 1010},{"Chennai", 1366},{"Bangalore", 1560},{"Hyderabad", 1503}, {"Delhi",1305 },{"Guwahati",519},{"Bhubaneshwar",367 } };
                adjacencyList["Bangalore"] = { {"Chennai", 284} ,{"Hyderabad", 503},{"Mumbai",842},{"Thiruvananthapuram",503},{"Chennai",621} };
                adjacencyList["Hyderabad"] = { {"Bangalore", 503}, {"Mumbai", 617},{"Chennai",521},{"Kolkata", 1503},{"Ahmedabad", 876} ,{"Bhubaneshwar",834 } };
                adjacencyList["Ahmedabad"] = { {"Hyderabad", 876} ,{"Jaipur",534}, {"Mumbai", 441},{"Delhi", 775} };
                adjacencyList["Jaipur"] = { {"Delhi", 241},{"Ahmedabad", 534} };
                adjacencyList["Lucknow"] = { {"Kolkata", 1010},{"Agra", 296},{"Delhi", 419},{"Bhubaneshwar",882 } };
                adjacencyList["Agra"] = { {"Lucknow",296 },{"Delhi", 180} };
                adjacencyList["Srinagar"] = { {"Delhi", 643} };
                adjacencyList["Guwahati"] = { {"Kolkata", 519} };
                adjacencyList["Bhubaneshwar"] = { {"Kolkata", 367},{"Lucknow",882}, {"Hyderabad",834} };
                adjacencyList["Thiruvananthapuram"] = { {"Bangalore", 503},{"Chennai",621} };


                // Render the map


                // Get user input for cities
              //  setFullscreen(visualizer.gWindow, visualizer.gRenderer);
                visualizer.renderMap();
                // Draw all cities
                for (const auto& city : cityPositions)
                {
                    visualizer.drawCity(city.first, city.second.x, city.second.y);
                }

                // Draw the path between the cities
                auto connections = adjacencyList.find(startCity);
                if (connections != adjacencyList.end())
                {
                    auto connection = find_if(connections->second.begin(), connections->second.end(),
                        [endCity](const pair<string, int>& p)
                        {
                            return p.first == endCity;
                        });

                    if (connection != connections->second.end())
                    {
                        const CityInfo& startCityInfo = cityPositions[startCity];
                        const CityInfo& endCityInfo = cityPositions[endCity];
                        visualizer.drawEdge(startCityInfo.x, startCityInfo.y, endCityInfo.x, endCityInfo.y);
                    }
                }

                // Find and draw the shortest path if it exists
                pair<vector<string>, int> result = dijkstra(startCity, endCity);

                if (result.first.size() < 2)
                {
                    cerr << "Error: Invalid shortest path." << endl;
                }
                SDL_RenderPresent(visualizer.gRenderer);
                // Draw the path between the cities
                system("cls");
                cout << dye::yellow("The path to take: "<<dye::yellow(startCity)) << "->";
                for (size_t i = 1; i < result.first.size(); ++i)
                {
                    if (i == result.first.size()-1)
                        cout << dye::yellow(result.first[i]);
                    else
                    cout << dye::yellow(result.first[i])<<dye::yellow("->");
                    SDL_Delay(1500);
                    const CityInfo& startCityInfo = cityPositions[result.first[i - 1]];
                    const CityInfo& endCityInfo = cityPositions[result.first[i]];
                    visualizer.drawEdge(startCityInfo.x, startCityInfo.y, endCityInfo.x, endCityInfo.y);
                    SDL_RenderPresent(visualizer.gRenderer);

                    // Delay for one second

                }

                // Update the window

                cout <<endl<< dye::yellow("Distance: ") << dye::yellow(result.second) << dye::yellow(" Kilometers. ") << endl;
                float amount = result.second * 10.4;
                cout << dye::yellow("Fare: Rs ") << dye::yellow(amount);
                // Wait for a key press before exiting
                bool quit = false;
                SDL_Event e;
                while (!quit)
                {
                    while (SDL_PollEvent(&e) != 0)
                    {
                        if (e.type == SDL_QUIT)
                        {
                            quit = true;
                        }
                        else if (e.type == SDL_KEYDOWN)
                        {
                            quit = true;
                        }
                    }
                }

                // Close SDL
                visualizer.closeSDL();
                goto menu;
            }
            if (counter == 3)
            {
                system("cls");

                cout << dye::red("List of cities: ") << endl;
                cout << dye::green("1. Delhi") << endl;
                cout << dye::green("2. Mumbai") << endl;
                cout << dye::green("3. Chennai") << endl;
                cout << dye::green("4. Kolkata") << endl;
                cout << dye::green("5. Bangalore") << endl;
                cout << dye::green("6. Hyderabad") << endl;
                cout << dye::green("7. Ahmedabad") << endl;
                cout << dye::green("8. Jaipur") << endl;
                cout << dye::green("9. Lucknow") << endl;
                cout << dye::green("10. Srinagar") << endl;
                cout << dye::green("11. Bhubaneshwar") << endl;
                cout << dye::green("12. Thiruvananthapuram") << endl;
                cout << dye::green("13. Agra") << endl;
                cout << dye::green("14. Guwahati") << endl << endl;
                string startCity;
                cout << dye::yellow("Enter the city you want to start from: ");
                int x;
                cin >> x;
                startCity = city(x);

                VisualizerCities visualizer;
                if (!visualizer.initSDL())
                {
                    cerr << "SDL initialization failed. Exiting..." << endl;
                    return 1;
                }

                // Initialize city positions
                initializeCities();
                adjacencyList["Delhi"] = { {"Srinagar", 643}, {"Ahmedabad", 775}, {"Jaipur", 241}, {"Lucknow",419 }, {"Agra",180 }, {"Kolkata",1305 } , {"Mumbai",1148 } };
                adjacencyList["Mumbai"] = { {"Bangalore", 987}, {"Ahmedabad", 525},{"Hyderabad", 617}, {"Delhi",1148 } };
                adjacencyList["Chennai"] = { {"Bangalore", 284},{"Hyderabad", 521} };
                adjacencyList["Kolkata"] = { {"Lucknow", 1010},{"Chennai", 1366},{"Bangalore", 1560},{"Hyderabad", 1503}, {"Delhi",1305 },{"Guwahati",519},{"Bhubaneshwar",367 } };
                adjacencyList["Bangalore"] = { {"Chennai", 284} ,{"Hyderabad", 503},{"Mumbai",842},{"Thiruvananthapuram",503},{"Chennai",621} };
                adjacencyList["Hyderabad"] = { {"Bangalore", 503}, {"Mumbai", 617},{"Chennai",521},{"Kolkata", 1503},{"Ahmedabad", 876} ,{"Bhubaneshwar",834 } };
                adjacencyList["Ahmedabad"] = { {"Hyderabad", 876} ,{"Jaipur",534}, {"Mumbai", 441},{"Delhi", 775} };
                adjacencyList["Jaipur"] = { {"Delhi", 241},{"Ahmedabad", 534} };
                adjacencyList["Lucknow"] = { {"Kolkata", 1010},{"Agra", 296},{"Delhi", 419},{"Bhubaneshwar",882 } };
                adjacencyList["Agra"] = { {"Lucknow",296 },{"Delhi", 180} };
                adjacencyList["Srinagar"] = { {"Delhi", 643} };
                adjacencyList["Guwahati"] = { {"Kolkata", 519} };
                adjacencyList["Bhubaneshwar"] = { {"Kolkata", 367},{"Lucknow",882}, {"Hyderabad",834} };
                adjacencyList["Thiruvananthapuram"] = { {"Bangalore", 503},{"Chennai",621} };
                // Initialize adjacency list based on your city positions and connections
                // (same adjacencyList initialization as in the "Travel" option)

                // Render the map and draw cities
                visualizer.renderMap();
                for (const auto& city : cityPositions)
                {
                    visualizer.drawCity(city.first, city.second.x, city.second.y);
                }
                SDL_RenderPresent(visualizer.gRenderer);

                // Perform BFS traversal with visualization
                bfsTraversalWithVisualization(startCity, visualizer);

                // Wait for a key press before exiting
                bool quit = false;
                SDL_Event e;
                while (!quit)
                {
                    while (SDL_PollEvent(&e) != 0)
                    {
                        if (e.type == SDL_QUIT)
                        {
                            quit = true;
                        }
                        else if (e.type == SDL_KEYDOWN)
                        {
                            quit = true;
                        }
                    }
                }

                // Close SDL
                visualizer.closeSDL();
                goto menu;
            }
            if (counter == 4)
            {
                system("cls");
                cout << dye::red("List of cities: ") << endl;
                cout << dye::green("1. Delhi") << endl;
                cout << dye::green("2. Mumbai") << endl;
                cout << dye::green("3. Chennai") << endl;
                cout << dye::green("4. Kolkata") << endl;
                cout << dye::green("5. Bangalore") << endl;
                cout << dye::green("6. Hyderabad") << endl;
                cout << dye::green("7. Ahmedabad") << endl;
                cout << dye::green("8. Jaipur") << endl;
                cout << dye::green("9. Lucknow") << endl;
                cout << dye::green("10. Srinagar") << endl;
                cout << dye::green("11. Bhubaneshwar") << endl;
                cout << dye::green("12. Thiruvananthapuram") << endl;
                cout << dye::green("13. Agra") << endl;
                cout << dye::green("14. Guwahati") << endl << endl;
                string startCity;
                cout << dye::yellow("Enter the city you want to start from: ");
                int x;
                cin >> x;
                startCity = city(x);

                VisualizerCities visualizer;
                if (!visualizer.initSDL())
                {
                    cerr << "SDL initialization failed. Exiting..." << endl;
                    return 1;
                }

                // Initialize city positions
                initializeCities();
                adjacencyList["Delhi"] = { {"Srinagar", 643}, {"Ahmedabad", 775}, {"Jaipur", 241}, {"Lucknow",419 }, {"Agra",180 }, {"Kolkata",1305 } , {"Mumbai",1148 } };
                adjacencyList["Mumbai"] = { {"Bangalore", 987}, {"Ahmedabad", 525},{"Hyderabad", 617}, {"Delhi",1148 } };
                adjacencyList["Chennai"] = { {"Bangalore", 284},{"Hyderabad", 521} };
                adjacencyList["Kolkata"] = { {"Lucknow", 1010},{"Chennai", 1366},{"Bangalore", 1560},{"Hyderabad", 1503}, {"Delhi",1305 },{"Guwahati",519},{"Bhubaneshwar",367 } };
                adjacencyList["Bangalore"] = { {"Chennai", 284} ,{"Hyderabad", 503},{"Mumbai",842},{"Thiruvananthapuram",503},{"Chennai",621} };
                adjacencyList["Hyderabad"] = { {"Bangalore", 503}, {"Mumbai", 617},{"Chennai",521},{"Kolkata", 1503},{"Ahmedabad", 876} ,{"Bhubaneshwar",834 } };
                adjacencyList["Ahmedabad"] = { {"Hyderabad", 876} ,{"Jaipur",534}, {"Mumbai", 441},{"Delhi", 775} };
                adjacencyList["Jaipur"] = { {"Delhi", 241},{"Ahmedabad", 534} };
                adjacencyList["Lucknow"] = { {"Kolkata", 1010},{"Agra", 296},{"Delhi", 419},{"Bhubaneshwar",882 } };
                adjacencyList["Agra"] = { {"Lucknow",296 },{"Delhi", 180} };
                adjacencyList["Srinagar"] = { {"Delhi", 643} };
                adjacencyList["Guwahati"] = { {"Kolkata", 519} };
                adjacencyList["Bhubaneshwar"] = { {"Kolkata", 367},{"Lucknow",882}, {"Hyderabad",834} };
                adjacencyList["Thiruvananthapuram"] = { {"Bangalore", 503},{"Chennai",621} };
                // Initialize adjacency list based on your city positions and connections
                // (same adjacencyList initialization as in the "Travel" option)

                // Render the map and draw cities
                visualizer.renderMap();
                for (const auto& city : cityPositions)
                {
                    visualizer.drawCity(city.first, city.second.x, city.second.y);
                }
                SDL_RenderPresent(visualizer.gRenderer);

                // Perform BFS traversal with visualization
                dfsTraversalWithVisualization(startCity, visualizer);

                // Wait for a key press before exiting
                bool quit = false;
                SDL_Event e;
                while (!quit)
                {
                    while (SDL_PollEvent(&e) != 0)
                    {
                        if (e.type == SDL_QUIT)
                        {
                            quit = true;
                        }
                        else if (e.type == SDL_KEYDOWN)
                        {
                            quit = true;
                        }
                    }
                }

                // Close SDL
                visualizer.closeSDL();
                goto menu;
            }
        }
    }

    goto menu;
    return 0;
}
