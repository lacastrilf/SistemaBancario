#include <iostream>
#include <string>
#include <cstdlib> // Para rand() y srand()
#include <cctype>  // Use std::isdigit
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>    //Para capturar el tiempo
#include <ctime>     // Para capturlar el tiempo
#include <numeric>   // Para iota
#include <random>    // Para random_device
#include <algorithm> // Para shuffle
#include <thread>

using namespace std;

// Include class
#include "Cliente.cpp"
#include "Nodo.cpp"
#include "List.cpp"

// Get the time

// Separete the time
string getTime()
{
    this_thread::sleep_for(chrono::seconds(1));
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::string timeString = std::ctime(&currentTime);
    string subString = timeString.substr(11, 8);
    return subString;
};

int chooseList(string userActive)
{

    if (userActive == "Si")
    {
        int randomNumber = rand() % 3 + 1;
        cout << "Si es activo: " << randomNumber << endl;
        return randomNumber;
    }
    else
    {
        int randomNumber = rand() % 2 + 3;
        cout << "No es activo: " << randomNumber << endl;
        return randomNumber;
    }
};

List emptyList(List list, Cliente client, string letter)
{
    if (list.getSize() <= 50)
    {
        cout << "La lista tiene espacios." << endl;
        string turn = letter + to_string(list.getTurn());
        client.setTurn(turn);
        client.setTimeStart(getTime());
        list.encolar(client);
        return list;
    }
    else
    {
        cout << "La lista no tiene espacios." << endl;
        return list;
    }
}

List transactionCompleted(List list, List listOut)
{
    if (list.getSize() != 0)
    {
        Nodo *temp = list.getFirst();
        temp->cliente.setTimeEnd(getTime()); // delete temp;Recordar crear el temp por fuera para poder hacerle delete sin que afecte el codigos
        listOut.pilar(temp->cliente);
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    else
    {
        cout << "No hay clientes para transaccionar." << endl;
    }
    return listOut;
};

Cliente getRandomClient(Cliente matrix[25][10])
{
    vector<int> uniqueNumbers(250);
    iota(uniqueNumbers.begin(), uniqueNumbers.end(), 0);

    random_device rd;
    mt19937 g(rd());

    shuffle(uniqueNumbers.begin(), uniqueNumbers.end(), g);

    int randomIndex = uniqueNumbers[0];
    int row = randomIndex / 10;
    int column = randomIndex % 10;

    Cliente selectedClient = matrix[row][column];

    return selectedClient;
}

List completedList(List list, List listOut)
{
    if (list.getSize() == 0)
    {
        cout << "La lista esta vacia." << endl;
        return listOut;
    }
    Nodo *temp = list.getFirst();
    int i = 0;
    while (i < list.getSize() && temp != nullptr)
    {
        temp->cliente.setTimeEnd(getTime());
        this_thread::sleep_for(chrono::milliseconds(100));
        listOut.pilar(temp->cliente);
        temp = temp->next;
        list.desencolar();
    }
    delete temp;
    return listOut;
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));
    const int totalClients = 250;
    Cliente matrix[25][10]{};
    List listA;
    List listB;
    List listC;
    List listD;
    List listOut;

    // Reads the clientes.txt file, separates by lines, by commas, and sorts the data by books in the stock.
    std::ifstream file("./txtFiles/clientes.txt");
    if (!file.is_open())
    {
        cerr << "Error opening file!" << endl;
        return 1;
    }
    else
    {
        string line, arrayTokens[7]{}, token;
        int row = 0, column = 0, index = 0;
        while (getline(file, line) && row < 50)
        {
            stringstream ss(line);
            string token;

            while (getline(ss, token, ',') && index < 7)
            {
                arrayTokens[index] = token;
                index++;
            }

            matrix[row][column].setName(arrayTokens[0]);
            matrix[row][column].setLastName(arrayTokens[1]);
            matrix[row][column].setId(arrayTokens[2]);
            matrix[row][column].setActive(arrayTokens[3]);

            column++;
            index = 0;

            if (column >= 10)
            {
                row++;
                column = 0;
            }
        }
    }

    file.close();

    int i = 0, j = 0;
    while (j < 10)
    {
        while (i < 5)
        {
            Cliente selectedClient = getRandomClient(matrix);
            int randomList = chooseList(selectedClient.getActive());
            if (randomList == 1)
            {
                listD = emptyList(listD, selectedClient, "D");
            }
            else if (randomList == 2)
            {
                listB = emptyList(listB, selectedClient, "B");
            }
            else if (randomList == 3)
            {
                listA = emptyList(listA, selectedClient, "A");
            }
            else if (randomList == 4)
            {
                listC = emptyList(listC, selectedClient, "C");
            }
            i++;
        }

        i = 0;
        listOut = transactionCompleted(listA, listOut);
        listA.desencolar();
        listOut = transactionCompleted(listB, listOut);
        listB.desencolar();
        listOut = transactionCompleted(listC, listOut);
        listC.desencolar();
        listOut = transactionCompleted(listD, listOut);
        listD.desencolar();
        j = j + 5;
        cout << j << endl;
    };

    // Funcion que mire sino esta vacia y de estarlo entones que la recorra toda y haga las transacciones
    listOut = completedList(listA, listOut);
    listOut = completedList(listB, listOut);
    listOut = completedList(listC, listOut);
    listOut = completedList(listD, listOut);

    std::ofstream outFile("./txtFiles/outClientes.txt");
    if (outFile.is_open())
    {
        Nodo *actual = listOut.getFirst();
        while (actual != nullptr)
        {
            outFile << actual->cliente.printInformation() << endl;
            actual = actual->next;
        }
        delete actual;
    }
    else
    {
        cout << "No se puede abrir el archivo" << endl;
    };

    return 0;
}