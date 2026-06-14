#include "Lib.h"

int main()
{
    Polynomial p1, p2;

    int choice;

    do
    {
        cout << "\n===== MENU =====\n";
        cout << "1. Enter first polynomial\n";
        cout << "2. Enter second polynomial\n";
        cout << "3. Show polynomials\n";
        cout << "4. Add\n";
        cout << "5. Multiply\n";
        cout << "6. Multiply by constant\n";
        cout << "7. Divide\n";
        cout << "8. Power\n";
        cout << "9. Pseudovariable m(a,b)=value\n";
        cout << "10. Save first polynomial\n";
        cout << "11. Load first polynomial\n";
        cout << "0. Exit\n";
        cout << "Choice: ";

        cin >> choice;
        cout << "\n";

        switch(choice)
        {
        case 1:
            cin >> p1;
            break;

        case 2:
            cin >> p2;
            break;

        case 3:
            setColor(9);
            cout << "P1 = " << p1 << endl;
            cout << "P2 = " << p2 << endl;
            setColor(7);
            break;

        case 4:
            setColor(9);
            cout << "Result: " << p1 + p2 << endl;
            setColor(7);
            break;

        case 5:
            setColor(9);
            cout << "Result: " << p1 * p2 << endl;
            setColor(7);
            break;

        case 6:
        {
            double k;
            cout << "Constant: ";
            cin >> k;

            setColor(9);
            cout << "\nResult: " << p1 * k << endl;
            setColor(7);
            break;
        }

        case 7:
        {
            setColor(9);
            try
            {
                cout << "Result: " << p1 / p2 << endl;
            }
            catch(const exception& e)
            {
                cout << "Error: " << e.what() << endl;
            }
            setColor(7);
            break;
        }

        case 8:
        {
            int n;
            cout << "Power: ";
            cin >> n;

            setColor(9);
            cout << "\nResult: " <<  p1.pow(n) << endl;
            setColor(7);
            break;
        }

        case 9:
        {
            int l, r;
            double value;

            cout << "Left degree: ";
            cin >> l;

            cout << "Right degree: ";
            cin >> r;

            cout << "Value: ";
            cin >> value;

            p1(l, r) = value;

            setColor(9);
            cout << "\nResult: " << p1 << endl;
            setColor(7);
            break;
        }

        case 10:
            p1.saveToFile("poly.txt");
            break;

        case 11:
            p1.loadFromFile("poly.txt");
            break;
        }

    } while(choice != 0);

    return 0;
}