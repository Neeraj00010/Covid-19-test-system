#define _CRT_SECURE_NO_WARNINGS

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>   
#include <random>
#include <map>

using namespace std;

template <typename S>
ostream& operator<<(ostream& os,
    const vector<S>& vector)
{
    // Printing all the elements
    // using <<
    for (auto element : vector) {
        os << element << " ";
    }
    return os;
}

struct Patient
{
    string name;
    string age;
    string postalCode;
    int posResult = 0;
    int negResult = 0;
    map<string, bool> testResults;
};

// Vector containing all patients
vector<Patient> patients;

// Map of age groups and number of positive tests
map<string, int> ageGroupMap;

// Map containing number of positive tests per region
map<string, int> postalCodeMap;

int main(int argc, const char* argv[]) {
    
    srand( (unsigned) time(0));

    sf::RenderWindow window{ {900, 740},"Covid-19 Test System" };
    tgui::Gui gui{ window };
    gui.loadWidgetsFromFile("form.txt");

    auto childWin = gui.get<tgui::ChildWindow>("childWin");
    auto childWindow = gui.get<tgui::ChildWindow>("ChildWindow");
    
    auto add = gui.get<tgui::Button>("addButton");
    auto remove = gui.get<tgui::Button>("removeButton");
    auto test = gui.get<tgui::Button>("testButton");
    auto stats = gui.get<tgui::Button>("statsButton");
    auto hotspots = gui.get<tgui::Button>("hotspotsButton");

    auto nameText = gui.get<tgui::TextArea>("nameArea");
    auto ageText = gui.get<tgui::TextArea>("ageArea");
    auto postalCodeText = gui.get<tgui::TextArea>("postalCodeArea");

    auto addListView = gui.get<tgui::ListView>("addListView");
    auto testListView = gui.get<tgui::ListView>("testListView");


    addListView->addColumn("Patient Name", 150, tgui::ListView::ColumnAlignment::Center);
    addListView->addColumn("Age", 45, tgui::ListView::ColumnAlignment::Center);
    addListView->addColumn("Postal Code", 80, tgui::ListView::ColumnAlignment::Center);

    testListView->addColumn("Date", 175, tgui::ListView::ColumnAlignment::Center);
    testListView->addColumn("Patient Name", 160, tgui::ListView::ColumnAlignment::Center);
    testListView->addColumn("Age", 40, tgui::ListView::ColumnAlignment::Center);
    testListView->addColumn("Postal", 70, tgui::ListView::ColumnAlignment::Center);
    testListView->addColumn("Test", 65, tgui::ListView::ColumnAlignment::Center);
    testListView->setEnabled(false);


    add->onClick([&] {

        // Create a new patient
        Patient patient;

        patient.name = (string)nameText->getText();
        patient.age = (string)ageText->getText();
        patient.postalCode = (string)postalCodeText->getText();

        // Add patient to vector
        patients.push_back(patient);

        addListView->addItem({ patient.name , patient.age, patient.postalCode });
        
        nameText->setText("");
        ageText->setText("");
        postalCodeText->setText("");

        cout << "Name: " << patient.name << endl;
        cout << "Age: " << patient.age << endl;
        cout << "Postal Code: " << patient.postalCode << '\n' << endl;

        // Add patient to map
        string postalCodePrefix = patient.postalCode.substr(0, 3);
        if (postalCodeMap.find(postalCodePrefix) == postalCodeMap.end()) {
            postalCodeMap.insert(pair<string, int>(postalCodePrefix, 0));
        }
        
        });

    remove->onClick([&] 
    {
            //std::cout << "\nNumber of Items in list view: " << addListView->getItemCount() << endl;

            //std::cout << "Index to delete value from: " << addListView->getSelectedItemIndex() << endl;
            string postalCodePrefix = patients[addListView->getSelectedItemIndex()].postalCode.substr(0, 3);

            postalCodeMap[postalCodePrefix] -= patients[addListView->getSelectedItemIndex()].posResult;

            patients.erase(patients.begin() + addListView->getSelectedItemIndex());

            //std::cout << "\nItems in list view: " << addListView->getItemRow(addListView->getSelectedItemIndex()) << endl;

            auto removingPatient = addListView->getItemRow(addListView->getSelectedItemIndex());

            addListView->removeItem(addListView->getSelectedItemIndex());

            //std::cout << patients.size() << endl;

    });

    test->onClick([&]
    {
            //Patient object for tested result list view
            Patient newPatient;
            
            auto start = std::chrono::system_clock::now();
            // Some computation here
            auto end = std::chrono::system_clock::now();

            std::chrono::duration<double> elapsed_seconds = end - start;
            std::time_t end_time = std::chrono::system_clock::to_time_t(end);

            auto currentDate = std::ctime(&end_time);

            bool PosNeg = (rand() % 100) < 5;

            auto stringRow = addListView->getItemRow(addListView->getSelectedItemIndex());
            
            int index = 0;

            for ( Patient p : patients) 
            {
                if (std::find(stringRow.begin(), stringRow.end(), p.name) != stringRow.end())
                {
                    newPatient.name = p.name;
                    newPatient.age = p.age;
                    newPatient.postalCode = p.postalCode;

                    if (PosNeg == 0)
                    {
                        patients[addListView->getSelectedItemIndex()].testResults.insert(pair<string, bool>(currentDate, false));
                        testListView->addItem({ currentDate, newPatient.name , newPatient.age, newPatient.postalCode, "Negative" });
                        p.negResult = p.negResult + 1;
                    }
                    else
                    {
                        string postalCodePrefix = p.postalCode.substr(0, 3);
                        postalCodeMap[postalCodePrefix] += 1;

                        // Calculate age group
                        int ageGroupStart = (stoi(p.age) / 5) * 5;
                        string ageGroup = to_string(ageGroupStart) + "-" + to_string(ageGroupStart + 5);
                        ageGroupMap[ageGroup] += 1;

                        patients[addListView->getSelectedItemIndex()].testResults.insert(pair<string, bool>(currentDate, true));
                        testListView->addItem({ currentDate, newPatient.name , newPatient.age, newPatient.postalCode, "Positive" });
                        p.posResult = p.posResult + 1;
                    }
                    
                }
                
            }

    });

    stats->onClick([&] 
    {
            if (!childWin->getParent()) {
                gui.add(childWin);
            }

            auto label = gui.get<tgui::Label>("statLabel");
            auto ageList = gui.get<tgui::ListView>("ageListView");

            ageList->removeAllItems();

            ageList->addColumn("Age Groups", 170, tgui::ListView::ColumnAlignment::Center);
            ageList->addColumn("Test Results", 170, tgui::ListView::ColumnAlignment::Center);

            // Output results
            for (const auto& ageGroup : ageGroupMap)
            {
                ageList->addItem({ ageGroup.first , (tgui::String)ageGroup.second });
            }

            childWin->setVisible(true);

    });

    hotspots->onClick([&] 
    { 
            if (!childWindow->getParent()) {
                gui.add(childWindow);
            }

            auto label = gui.get<tgui::Label>("hotspotsLabel");
            auto hotList = gui.get<tgui::ListView>("hotListView");

            hotList->removeAllItems();

            hotList->addColumn("Hotsopts Zone", 170, tgui::ListView::ColumnAlignment::Center);
            hotList->addColumn("Test Results", 170, tgui::ListView::ColumnAlignment::Center);

            // Output results
            for (const auto& postalCode : postalCodeMap)
            {
                hotList->addItem({ postalCode.first , (tgui::String)postalCode.second });
            }

            childWindow->setVisible(true);
    });

    gui.mainLoop();
    
    return 0;
}
