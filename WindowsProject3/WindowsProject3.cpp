// WindowsProject3.cpp : Defines the entry point for the application.
//

/*
CODIGO PARA GENERAR DIALOGOS EN WINAPI
Y GESTIONAR LISTAS LIGADAS

*/

#include "framework.h"
#include "WindowsProject3.h"
#include <commctrl.h> 
#include <fstream>
#include <CommDlg.h>
using namespace std;

#define MAX_LOADSTRING 100

// Estructura y variables globales para la LL Alumnos
struct nodo_alumnos {
    int matricula;
    int costo;
    char nombre[20];
    char genero[1];
    char foto[MAX_PATH];
    int  fechanac;
    nodo_alumnos* ant;
    nodo_alumnos* sig;
};
nodo_alumnos* aux = 0, * lista_alumnos = 0, * ultimo_alumno = 0, * nuevo_alumno = 0;

// variables globales
char arch_alumnos[] = "boletos.dat";
char mensaje[50] = { 0 };
char textolist[40];
char nombrearchivo[MAX_PATH] = { 0 };
bool _login = false;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Doctor(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    AgregarAlumno(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ListaAlumnos(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Multimedia(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Login(HWND, UINT, WPARAM, LPARAM);

//Prototipos de funciones
void leer_archivo(char* archivo);
void escribir_archivo(char* archivo);
void agregar_alumno(nodo_alumnos* aux);
void eliminar_alumno(nodo_alumnos* aux);
nodo_alumnos* BuscarAlumno(int matr);
void Examinar(HWND hDlg, short tipo);
void CargarImagen(HWND hDlg, char file[], int Pic);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT3));

    // leer el archivo
    leer_archivo(arch_alumnos);

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    // guardar el archivo con cada nodo de la lista ligada
    escribir_archivo(arch_alumnos);

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ID_ALUMNO_AGREGAR:
                if (_login) {
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMVIEW), hWnd, AgregarAlumno);
                }
                else
                {
                    MessageBox(0, "Debe conectarse primero", "ERROR",
                        MB_ICONEXCLAMATION + MB_OK);
                }
                break;
            case ID_ALUMNO_LISTAALUMNOS:    // ID del Menú
                DialogBox(                  // Función para abrir una ventana
                    hInst, 
                    MAKEINTRESOURCE(IDD_LISTA_ALUMNOS), 
                    // Es el ID de la ventana
                    hWnd, 
                    ListaAlumnos);// Es la CALLBACK asociada a la ventana
                break;
            case ID_FILE_MULTIMEDIA:    // ID del Menú
                DialogBox(                  // Función para abrir una ventana
                    hInst,
                    MAKEINTRESOURCE(IDD_OPENFILE),
                    // Es el ID de la ventana
                    hWnd,
                    Multimedia);// Es la CALLBACK asociada a la ventana
                break;            
            case ID_FILE_LOGIN:    // ID del Menú
                DialogBox(                  
                    hInst,
                    MAKEINTRESOURCE(IDD_FORM_LOGIN),
                    hWnd,
                    Login);// Es la CALLBACK asociada a la ventana
                    break;    
            case ID_HELP_INFORMACI32777:
                DialogBox(
                    hInst,
                    MAKEINTRESOURCE(IDD_DIALOG1),
                    hWnd,
                    Doctor);// Es la CALLBACK asociada a la ventana
                break;
                        
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Doctor(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK AgregarAlumno(
            HWND hDlg, 
            UINT message, 
            WPARAM wParam, 
            LPARAM lParam)
{
    static HWND HWtel;
    static HWND HWnombre;
    static HWND HWmascota;
    static HWND HWconsulta;
    static HWND HWcosto;
    static HWND HWestatus;

    

    nodo_alumnos alumno_temp;
    char matricula[20] = { 0 };
    char costo[30] = { 0 };

    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        


        HWtel =  GetDlgItem(hDlg, IDC_EDIT1);
        HWnombre = GetDlgItem(hDlg, IDC_EDIT2);
        HWcosto = GetDlgItem(hDlg, IDC_EDIT5);
        
        
        SendMessage(HWtel, WM_SETTEXT, 0, (LPARAM)"captura numero");
        SendMessage(HWnombre, WM_SETTEXT, 0, (LPARAM)"captura nombre");
        SendMessage(HWcosto, WM_SETTEXT, 0, (LPARAM)"captura precio del boleto");
        
        // inicializar el radio que debe
        // estar seleccionado, un default
        CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
        return (INT_PTR)TRUE;

        //Pendiente: Leer fecha seleccionada

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_EXIT)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }

        // prender y apagar
        // segun se seleccione cada radio button
        // de forma manual

        if (LOWORD(wParam) == IDC_RADIO1)
        {
            CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);

            // Muestra un mensaje cuando se selecciona IDC_RADIO1
            MessageBox(0, "Ingrese un costo de entre 200-400", "Aviso", MB_ICONINFORMATION | MB_OK);

            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_RADIO2)
        {
            CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO2);

            // Muestra un mensaje cuando se selecciona IDC_RADIO1
            MessageBox(0, "Ingrese un costo de entre 300-500", "Aviso", MB_ICONINFORMATION | MB_OK);

            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_RADIO3)
        {
            CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO3);

            // Muestra un mensaje cuando se selecciona IDC_RADIO1
            MessageBox(0, "Ingrese un costo de entre 400-600", "Aviso", MB_ICONINFORMATION | MB_OK);

            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_RADIO1)
        {
            CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_RADIO2)
        {
            CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO2);
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_RADIO3)
        {
            CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO3);
            return (INT_PTR)TRUE;
        }
        

       



        // aquí va el código que quiero que se ejecute
        // cuando el usuario le da clic al botón

                                // Id del botón
        if (LOWORD(wParam) == IDC_ADD_STUDENT
            && HIWORD(wParam) == BN_CLICKED)
        {            
            // Obtener el Windows Handler de cada objeto
            HWtel = GetDlgItem(hDlg, IDC_EDIT1);
            HWnombre = GetDlgItem(hDlg, IDC_EDIT2);
            
            // mandano un mensaje
            // para leer lo que el usuario capturó 
            // en las cajas de texto: Edit Control
            // se pasa a las variables
            SendMessage(HWtel, WM_GETTEXT, 20, (LPARAM)matricula);
            SendMessage(HWcosto, WM_GETTEXT, 30, (LPARAM)costo);
            SendMessage(HWnombre, WM_GETTEXT, 20, (LPARAM)alumno_temp.nombre);
            
            
            if (strlen(matricula) == 0 || strlen(costo) == 0 || strlen(alumno_temp.nombre) == 0)  
            {
                MessageBox(0, "Por favor, llene todos los campos.", "Error", MB_ICONERROR | MB_OK);
                return (INT_PTR)TRUE; // No continuar si algún campo está vacío
            }


            // convirtiendo de char a integer
            alumno_temp.matricula = atoi(matricula);
            alumno_temp.costo = atoi(costo);

            if (IsDlgButtonChecked(hDlg, IDC_RADIO2) == BST_CHECKED)
            {
                
                alumno_temp.costo = static_cast<int>(alumno_temp.costo * 0.9);
            }
            
            alumno_temp.costo = static_cast<int>(alumno_temp.costo * 1.16);

            // Crear nuevo nodo
            nodo_alumnos* nuevo = new nodo_alumnos;
            nuevo->matricula = alumno_temp.matricula;
            nuevo->costo = alumno_temp.costo;
            

            strcpy(nuevo->nombre, alumno_temp.nombre);
            

            if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
                strcpy(nuevo->genero, "H");
            if (IsDlgButtonChecked(hDlg, IDC_RADIO2) == BST_CHECKED)
                strcpy(nuevo->genero, "M");
            if (IsDlgButtonChecked(hDlg, IDC_RADIO3) == BST_CHECKED)
                strcpy(nuevo->genero, "C");

            strcpy(nuevo->foto, "");
            nuevo->fechanac = 0;


            // AGREGAR NODO
            agregar_alumno(nuevo);
            // prepara el mensaje
            strcpy(mensaje, "Boleto de: ");
            strcat(mensaje, alumno_temp.nombre);
            strcat(mensaje, ", ha sido comprado");

            MessageBox(0, mensaje, "VENTA",
                MB_ICONINFORMATION + MB_OK);
            //limpiar las cajitas de texto
            SendMessage(HWtel, WM_SETTEXT, 0, (LPARAM)"");
            SendMessage(HWnombre, WM_SETTEXT, 0, (LPARAM)"");
            SendMessage(HWcosto, WM_SETTEXT, 0, (LPARAM)"");
            


            return (INT_PTR)TRUE;
        }


        break;
    }
    return (INT_PTR)FALSE;
}


// Message handler for about box.
INT_PTR CALLBACK ListaAlumnos(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    nodo_alumnos* temp = 0;
    nodo_alumnos alumno_temp;

    int idx = 0;
    int indice = 0;
    int matricula = 0;
    int costo1 = 0;
    char s_matr[20] = { 0 };
    char nombre[200] = { 0 };
    char Costo[200] = { 0 };
    

    
    
    static HWND hEditMatricula;
    static HWND hEditCosto;

    switch (message)
    {
    case WM_INITDIALOG:
        
        hEditMatricula = GetDlgItem(hDlg, IDC_EDITMATR);
        hEditCosto = GetDlgItem(hDlg, IDC_EDITCOSTO);

        //SendDlgItemMessage(hDlg, IDC_LIST2, LB_RESETCONTENT, 0, 0);
        //SendDlgItemMessage(hDlg, IDC_CBO_ALUMNOS, CB_RESETCONTENT, 0, 0);
        //leer la lista ligada, nodo por nodo
        idx = 0;
        aux = lista_alumnos;
        while (aux != 0)
        {
            // por cada nodo, agregar al LISTBIOX             ​
            SendDlgItemMessage(hDlg, IDC_LIST2, LB_ADDSTRING, 
                                0, 
                                (LPARAM)aux->nombre);
            //IDC_CBO_ALUMNOS
            SendDlgItemMessage(hDlg, IDC_CBO_ALUMNOS, CB_ADDSTRING,
                                0,
                                (LPARAM)aux->nombre);
            // asociar la matricula como dato adicional del ListBox
            SendDlgItemMessage(hDlg, IDC_LIST2, LB_SETITEMDATA,
                idx,                
                aux->matricula);
            SendDlgItemMessage(hDlg, IDC_CBO_ALUMNOS, CB_SETITEMDATA,
                idx,
                aux->matricula);
            
            
            idx++;
            aux = aux->sig;
        }



        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_BUTTON3)
        {
            indice = 0;

            indice = SendDlgItemMessage(hDlg, IDC_LIST2, LB_GETCURSEL, 0, 0);

            if (indice != LB_ERR) {

                SendDlgItemMessage(hDlg, IDC_LIST2, LB_GETTEXT, indice, (LPARAM)textolist);

                SendMessage(GetDlgItem(hDlg, IDC_LIST2), LB_DELETESTRING, (WPARAM)indice, 0);

                nodo_alumnos* actual = lista_alumnos;
                while (actual != nullptr && strcmp(actual->nombre, textolist) != 0)
                {
                    actual = actual->sig;
                }

                // Verifica si se encontró el nodo
                if (actual != nullptr)
                {
                    // Utiliza la función eliminar_alumno para eliminar el nodo de la lista vinculada
                    eliminar_alumno(actual);
                }
            }

            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_OK)
        {
            idx = SendDlgItemMessage(hDlg, IDC_CBO_ALUMNOS, CB_GETCURSEL, 0, 0);
            SendDlgItemMessage(hDlg, IDC_CBO_ALUMNOS, CB_GETLBTEXT, idx, (LPARAM)nombre);
            matricula = SendDlgItemMessage(hDlg, IDC_CBO_ALUMNOS, CB_GETITEMDATA, idx, 0);
            

            _itoa(matricula, s_matr, 10);
            
            strcat(nombre, " - ");
            strcat(nombre, s_matr);
            MessageBox(0, nombre, "LISTA BOLETOS", MB_ICONINFORMATION + MB_OK);




            strcpy(nombre, "");
            // tengo matricula
            temp = BuscarAlumno(matricula);
            if (temp != 0)
            {
                if (strcmp(aux->genero, "H") == 0) {
                    CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
                }
                if (strcmp(aux->genero, "M") == 0) {
                    CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO2);
                }
                if (strcmp(aux->genero, "C") == 0) {
                    CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO3);
                }

                
                _itoa(temp->costo, Costo, 10);

                // Asigna la matrícula y la mascota a los controles de edición
                SetWindowText(hEditMatricula, s_matr);
                SetWindowText(hEditCosto, Costo); 

            }

            //Agregar el tercer radio button, leer matricula ingresada en el static
            return (INT_PTR)TRUE;
        }
        
        break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK Multimedia(
            HWND hDlg, UINT message, 
            WPARAM wParam, LPARAM lParam)
{

    short tipoarchivo = 0;

    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            if (!_login) {
                MessageBox(0, "Debe conectarse primero", "ERROR",
                    MB_ICONEXCLAMATION + MB_OK);
                EndDialog(hDlg, LOWORD(wParam));
            }

            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDC_BROWSE)
            {
                if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
                    tipoarchivo = 1;    // MP3
                if (IsDlgButtonChecked(hDlg, IDC_RADIO2) == BST_CHECKED)
                    tipoarchivo = 2;    // BMP


                Examinar(hDlg, tipoarchivo);

                if (strcmp(nombrearchivo, "") != 0)
                {
                    SendDlgItemMessage(hDlg, IDC_FILENAME, WM_SETTEXT, 0, (LPARAM)nombrearchivo);

                    // IDC_IMAGEN
                    CargarImagen(hDlg, nombrearchivo, IDC_IMAGEN);


                }

                return (INT_PTR)TRUE;
            }
            break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for Login Form. Oct-27
INT_PTR CALLBACK Login(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

    char user[200] = {};
    char pass[200] = {};
    char texto[200] = {};

    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_OK && HIWORD(wParam) == BN_CLICKED)
        {
            // Leer el contenido de los EditControl
            // Buscar en el Archivo - Usuarios
            // si coincide, hacer true a la variable _login
            _login = true;
            strcpy(texto, "Bienvenido al sistema de venta: ");
            strcat(texto, user);
            MessageBox(0, texto, "MENSAJE",
                MB_ICONINFORMATION + MB_OK);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_PSW && HIWORD(wParam) == BN_CLICKED)
        {
            SendMessage(hDlg,DM_SETDEFID,(WPARAM)IDC_PASS,               (LPARAM)0);
            //SendMessage(NULL, EM_GETPASSWORDCHAR, IDC_PASS
        }
        if (LOWORD(wParam) == IDC_CERRAR && HIWORD(wParam) == BN_CLICKED)
        {
            // Leer el contenido de los EditControl
            // Buscar en el Archivo - Usuarios
            // si coincide, hacer true a la variable _login
            _login = false;
            MessageBox(0, "Usuario no existe y/o contraseña incorrecta", "MENSAJE",
                MB_ICONERROR + MB_OK);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//Definición de funciones
void leer_archivo(char* archivo) {
    ifstream arch_alumnos;
    arch_alumnos.open(archivo, ios::binary);
    if (arch_alumnos.is_open())
    {
        // leer el archivo, hasta el fin 
        nuevo_alumno = new nodo_alumnos;
        arch_alumnos.read((char*)nuevo_alumno, sizeof(nodo_alumnos));
        while (!arch_alumnos.eof())
        {
            aux = new nodo_alumnos;
            aux = nuevo_alumno;

            agregar_alumno(aux);

            nuevo_alumno = new nodo_alumnos;
            arch_alumnos.read((char*)nuevo_alumno, sizeof(nodo_alumnos));
        }
        arch_alumnos.close();
        //printf("archivo leído, lista ligada de alumnos creada\n");
        //system("pause");
    }
}

void escribir_archivo(char* archivo) {
    ofstream archi;
    archi.open(archivo, ios::trunc | ios::binary);
    if (archi.is_open())
    {
        // recorrer toda la lista ligada, nodo x nodo
        aux = lista_alumnos;
        while (aux != 0)
        {
            //guardar en el archivo cada nodo
            archi.write((char*)aux, sizeof(nodo_alumnos));

            //leer el siguiente nodo
            aux = aux->sig;
        }
        archi.close();
        //printf("Archivo guardado\n\n");
    }
    if (archi.fail())
    {
        MessageBox(0, "NO SE PUDO CREAR EL ARCHIVO", "MENSAJE",
            MB_ICONERROR + MB_OK);
    }
}

void agregar_alumno(nodo_alumnos* aux) {
    aux->ant = 0;
    aux->sig = 0;
    if (lista_alumnos == 0) { // está vacía
        lista_alumnos = aux;
    }
    else { // no está vacía
        ultimo_alumno->sig = aux;
        aux->ant = ultimo_alumno;
    }
    ultimo_alumno = aux;

}

void eliminar_alumno(nodo_alumnos* aux) {
    // Verificar si el nodo a borrar es el primero
    if (aux == lista_alumnos) {
        lista_alumnos = aux->sig;
        if (lista_alumnos != nullptr) {
            lista_alumnos->ant = nullptr;
        }
    }
    else {
        // Verificar si el nodo a borrar es el último
        if (aux == ultimo_alumno)
        {
            ultimo_alumno = aux->ant;
            if (ultimo_alumno != nullptr) {
                ultimo_alumno->sig = nullptr;
            }
        }
        else
        {
            // Modificar ant y sig de los nodos vecinos
            nodo_alumnos* prev = aux->ant;
            nodo_alumnos* next = aux->sig;
            if (prev != nullptr) {
                prev->sig = aux->sig;
            }
            if (next != nullptr) {
                next->ant = aux->ant;
            }
        }
    }
    delete aux;
}

nodo_alumnos* BuscarAlumno(int matr) {
    nodo_alumnos* alum = 0;
    aux = 0;
    aux = lista_alumnos;
    while (aux != 0)
    {
        if (aux->matricula == matr) {
            alum = aux;
            break;
        }
        aux = aux->sig;
    }
    return alum;
}

void Examinar(HWND hDlg, short tipo) {
    char szFile[MAX_PATH] = { 0 };
    char tipos[200] = { 0 };
                        // buffer for file name​
    OPENFILENAME ofn;       // common dialog box structure​
    HANDLE hf;              // file handle​

    if (tipo == 1)  // MP3
    {
        strcpy(tipos, "Todos\0*.*\0Audio MP3\0*.mp3");
    }
    
    else {
        if (tipo == 2)  // BMP
        {
            strcpy(tipos, "Todos\0*.*\0Mapa de Bits\0*.bmp");
        }
        else // TODOS
        {
            strcpy(tipos, "Todos\0*.*\0Archivos Texto\0*.TXT\0Archivos Word (97-2003)\0*.doc\0Archivos Word\0*.docx\0Imagenes jpg\0*.jpg\0Audio MP3\0*.mp3\0Mapa de Bits\0*.bmp");
        }
    }
    strcpy(tipos, "Todos\0*.*\0Archivos Texto\0*.TXT\0Archivos Word (97-2003)\0*.doc\0Archivos Word\0*.docx\0Imagenes jpg\0*.jpg\0Audio MP3\0*.mp3\0Mapa de Bits\0*.bmp");

    // Initialize OPENFILENAME​

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hDlg;
    ofn.lpstrFile = szFile;

        // Set lpstrFile[0] to '\0' so that GetOpenFileName does not ​

        // use the contents of szFile to initialize itself.​

    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrFilter = "Todos\0*.*\0Archivos Texto\0*.TXT\0Archivos Word (97-2003)\0*.doc\0Archivos Word\0*.docx\0Imagenes jpg\0*.jpg\0Audio MP3\0*.mp3\0Mapa de Bits\0*.bmp";

    ofn.nFilterIndex = 2;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. ​
    if (GetOpenFileName(&ofn) == TRUE)    {
        //
        strcpy(nombrearchivo, ofn.lpstrFile);
    }
    else {
        strcpy(nombrearchivo, "");
        MessageBox(hDlg, "No eligió archivo", "Aviso", MB_OK | MB_ICONINFORMATION);
    }
    //return szFile;
}

void CargarImagen(HWND hDlg, char file[], int Pic) {
    static HBITMAP bmp1, bmp2;

    //Al objeto bmp1, se le asigna sin imagen:
    bmp1 = (HBITMAP)SendDlgItemMessage(hDlg, Pic, STM_GETIMAGE, IMAGE_BITMAP, 0);

    //Al objeto bmp2, se le asigna una imagen local:
    bmp2 = (HBITMAP)LoadImage(NULL, file, IMAGE_BITMAP, 200, 200, LR_LOADFROMFILE);

    SendDlgItemMessage(
        hDlg,
        Pic,
        STM_SETIMAGE,
        IMAGE_BITMAP,
        (LPARAM)bmp2);
}