#include <SPI.h>					// Import SPI Lib
#include <SdFat.h>					// Import SD card Lib
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>

SdFat sd;							// SD Card Object
SFEMP3Shield MP3player;				// MP3 Shield Object
const int chipselect = 9;

#define c_min 1 					// min numero de canciones en sd
#define c_max 9					    //max numero de canciones en sd

uint8_t v_min= 60;
uint8_t v_max= 30;
int track_n= 0;
int track_l= 0;

void setup() {						//Se setea la comunicación serial

	Serial.begin(115200);
	sd.begin(chipselect,SPI_HALF_SPEED);
	if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) {sd.initErrorHalt();}
	MP3player.begin();
	MP3player.setVolume(v_min);
	if(MP3player.begin() != 0) //Prueba de error!!
	{

		uint8_t error1 = MP3player.begin(); //Acá estoy guardando el número de error que me saca al tratar de inicializar
		Serial.print(error1);				//El error que saca es el 6 que dice "6 Patch was not loaded successfully. This may result in playTrack errors"

	}
	
}


void loop()
{
	SeeknPlay();
}


void SeeknPlay() 										//Método principal que permite buscar, validar y repoducir la canción
{
	String inp = selectartist(); 						//Llama el método que trae el artista seleccionado y lo guarda en una variable string
	int cont = 0;										
	uint8_t track_n;
	while(cont == 0)
	{
	bool encontrado = false;
	int counter = 0;
	
	while(!encontrado)
	{
		randomSeed(millis());                            //inicializa el generador de números aleatorios con la funcion millis para que sea siempre diferente en cada corrida
		uint8_t track_n = (random(c_min,c_max+1));				 //Selecciona el número aleatorio entre el rango máximo de canciones
	    while (track_n == track_l)						 //Mientras que el número de canción sea igual que el anterior selecciona otro
	    { 
	      	uint8_t track_n = (random(c_min, c_max + 0.99));

	    }
	    MP3player.playTrack(track_n);					 //Inicia la controlducción de la canción que seleccionó para obtener el dato del artista
		if(MP3player.playTrack(track_n) != 0) //Prueba de error!!
		{
			uint8_t error1 = MP3player.playTrack(track_n); //Acá estoy guardando el número de error que me saca al tratar de inicializar
			Serial.print(error1);				         //El error que saca es el 6 que dice "6 Patch was not loaded successfully. This may result in playTrack errors"
		}
		char memory[16];								 //Separa un espacio de memoria de 16 bits para el artista
		MP3player.trackArtist(memory);					 //Almacena el artista en el espacio de memoria
		MP3player.stopTrack();							 
		if(String(memory) == inp)						 //Convierte lo que haya en la memoria a un string, y lo compara con el artista seleccionado
		{
			encontrado = true;						     //Si el artista que obtuvo es el mismo que el seleccionado entonces guarda el status
		}
	}

	while(encontrado)									 //Si ya se encontró la canción del artista que se seleccionó, inicia la controlducción
	{
	
		MP3player.playTrack(track_n);					 //Inicia la controlducción
		Serial.print("\nTrack ");						 							
		Serial.print(track_n);							 //Muestra el número de la pista que está controlduciendo
		track_l = track_n;								 //Iguala las dos canciones para poder luego comparar para no repetirlas		
		char memory[16];
		MP3player.trackTitle(memory);					 //Obtiene el título de la canción y lo almacena en la memoria
		Serial.print(" - ");					
		Serial.println(memory);
		Serial.println("\nEscriba 'menu' para acceder a las opciones de control de reproducción)");


		while(MP3player.isPlaying())					 //Mientras esté en controlducción
		{
			String control = readString();				 //Llama el método que obtiene lo que haya en el puerto serial y lo almacena
			if(control == "cambiar")
			{
				encontrado = false;
				cont = 1;
	 			MP3player.stopTrack();	
			}
			if(control == "+")							 //Subir volumen
			{
					v_min = v_min-10;
					MP3player.setVolume(v_min);		
			}
			if(control == "-")							 //Bajar volumen
			{
				v_min = v_min+10;
				MP3player.setVolume(v_min);
			}
			if(control == "siguiente")					 //Para cambiar de artista
			{
				encontrado = false;						 //Redefine la variable encontrado para que reinicie el ciclo de búsqueda
				MP3player.stopTrack();
			}
			if(control == "pausa")						 //Pausar	
			{
				MP3player.pauseMusic();
			}
			if(control == "play")						 //Continuar reproducción
			{
				MP3player.resumeMusic();
			}
									
			if(control == "menu")
			{
				Serial.println("\n----MENU----");
				Serial.println("\nPara cambiar de artista escriba       'cambiar'");
				Serial.println("\nPara controlar volumen                '+' o '-'");
				Serial.println("\nPara cambiar la canción escriba       'siguiente'");
				Serial.println("\nPara pausar la controlducción escriba   'pausa'");
				Serial.println("\nPara reanudar la controlducción escriba 'play'");
			}
		}

		if(!MP3player.isPlaying())						 //Si la reproducción ha terminado, redefine la variable encontrado para reiniciar el programa
		{
			encontrado = false;
		}
	}
	}
}

String selectartist()	//Método que pregunta por el artista que se quiere escuchar
{
  Serial.println("\nSeleccione el artista que quiere escuchar. ");
  Serial.println("\n1. AndOne");
  Serial.println("\n2. Melotron");
  Serial.println("\n3. Front242");
  String art = "";
  while (art == "")
  {
      art = readString();
  }
  return art;
}


String readString()		//Método que permite obtener, concatenar y almacenar lo que llegue por el puerto serial
{
  String output = "";
  while (Serial.available())
  {
    output.concat(char(Serial.read()));
    delay(10);
  }
  return output;

}







