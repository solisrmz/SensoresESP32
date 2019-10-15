#include <WiFi.h>
#include <WebServer.h>
#include "DHTesp.h"
#define DHTpin 15
#define YL69 39
DHTesp dht;

// cambiar esto para cada red
const char* ssid = "INFINITUM579C";
const char* password = "7mkzB994Bm";
// definir el puerto de escucha del servidor
WebServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(YL69,INPUT);
  delay(50);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // iniciar la conexion Wifi
  WiFi.begin(ssid, password);
  // Verificar estado de la conexión
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // rutas de entrada
  server.on("/", getIndex);
  server.on("/Mediciones", getMediciones);
  server.on("/Adios", getAdios);
  server.onNotFound(salidaDefault);

  server.begin();
  Serial.println("HTTP server started");
  dht.setup(DHTpin, DHTesp::DHT11);
  
}
void loop() {
  // controlar cada petición realizadas por el cliente de forma contínua    
  server.handleClient();
}
  
void getIndex() {
    server.send (200, "text/html",escribirIndexHTML());
}

void getMediciones() {
  //PARA MEDIR LA TEMPERATURA DEL AMBIENTE
  Serial.println("Accediendo a datos ...");
  delay(dht.getMinimumSamplingPeriod());
  
  float humedad = dht.getHumidity();
  float temperatura = dht.getTemperature();

  if(isnan(humedad) || isnan(temperatura)){
    Serial.println("¡No se pudo leer sensor DHT!");
    return;
  }
  Serial.println("Estado\tHumedad(%)\tTemperatura(C)\tTemperatura(F)\tIndice de calor");
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humedad, 1);
  Serial.print("\t\t");
  Serial.print(temperatura, 1);
  Serial.print("\t\t");
  Serial.print(dht.toFahrenheit(temperatura), 1);
  Serial.print("\t\t");
  Serial.print(dht.computeHeatIndex(temperatura, humedad, false), 
  1);
  Serial.print("\t\t");
  Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperatura), 
  humedad, true), 1);


  //LECTURA TIERRA
  int lectura = analogRead(YL69);
  Serial.println("Accediendo a datos ...");
  String estado;
  Serial.print("La lectura es: ");
  Serial.println(lectura);
    
  int lecturaPorcentaje = map(lectura, 4095, 1200, 0, 100);

  Serial.print("La Humedad es del: ");
  Serial.print(lecturaPorcentaje);
  Serial.println("%");

  // lecturas entre 4095 - 1023
  if (lectura >= 4095){
      estado = "EL SENSOR ESTA DESCONECTADO O FUERA DEL SUELO"; 
  }
  else if (lectura <4000 && lectura >= 3500){
      estado = "EL SUELO ESTA SECO";
  }
  else if (lectura < 3500 && lectura >= 1500){
      estado = "EL SUELO ESTA HUMEDO";
  }
  else if (lectura < 1500){
      estado = "EL SUELO ESTA MOJADO";
  } 
  delay(1000);
  server.send(200, "text/html", escribirHTML(temperatura, humedad, lecturaPorcentaje, estado));

}

//OTRA FUNCION

void getAdios(){
    server.send(404, "text/plain", "Lastima la solicitud no puede ser procesada :-(");
}
//OTRA FUNCION
void salidaDefault() {
  server.send(404, "text/plain", "Lastima la solicitud no puede ser procesada :-(");
}


//INDEX
String escribirIndexHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr +="<link rel=\"shortcut icon\" href=\"http://www.iconj.com/ico/z/0/z0vgrqt846.ico\" type=\"image/x-icon\">";
  ptr +="<title>Sap OS</title>\n";
  ptr +="<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #333333;}\n";
  ptr +="body{margin-top: 50px;}\n";
  ptr +="h1 {margin: 50px auto 30px;}\n";
  ptr +="h2 { font-size: 1.5rem; }\n";
  ptr +=".h2 { font-size: 3.0rem; }\n";
  ptr +=".p{font-size: 1.5 rem; }\n";
  ptr +=".img2 {width:100%;}\n";
  ptr +=".side-by-side{display: inline-block;vertical-align: middle;position: relative;}\n";
  ptr +=".humidity-icon{background-color: #3498db;width: 30px;height: 30px;border-radius: 50%;line-height: 36px;}\n";
  ptr +=".humidity-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".humidity{font-weight: 300;font-size: 60px;color: #3498db;}\n";
  ptr +=".temperature-icon{background-color: #f39c12;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;}\n";
  ptr +=".temperature-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".temperature{font-weight: 300;font-size: 60px;color: #f39c12;}\n";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;right: -20px;top: 15px;}\n";
  ptr +=".data{padding: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<img class=\"img2\" src=\"https://i.ibb.co/f84ryX2/vivero.jpg\" alt=\"vivero\" border=\"0\"></a>"; 
  ptr +="<h2 class=\"h2\">Sap OS</h2>\n";
  ptr +="<h2>BIENVENIDO</h2>\n";
  ptr +="<p class=\"p\" >Monitorea la temperatura, humedad y calidad del suelo de tus plantas</p>\n";
  ptr +="<img src=\"https://i.ibb.co/T4ZNzws/performance-1.png\" alt=\"performance\" border=\"0\">";
  ptr +="<h3>";
  ptr+= "<a href=\"/Mediciones\">Monitorear</a>"; 
  ptr +="</h3>";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}


//PAGINA HTML
String escribirHTML(float temperatura, float humedad, int lectura, String estado) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr +="<title>MONITOREA TUS PLANTAS</title>\n";
  ptr +="<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #333333;}\n";
  ptr +="body{margin-top: 50px;}\n";
  ptr +="h1 {margin: 50px auto 30px;}\n";
  ptr +=".side-by-side{display: inline-block;vertical-align: middle;position: relative;}\n";
  ptr +=".humidity-icon{background-color: #3498db;width: 30px;height: 30px;border-radius: 50%;line-height: 36px;}\n";
  ptr +=".humidity-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".humidity{font-weight: 300;font-size: 60px;color: #3498db;}\n";
  ptr +=".temperature-icon{background-color: #f39c12;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;}\n";
  ptr +=".temperature-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".temperature{font-weight: 300;font-size: 60px;color: #f39c12;}\n";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;right: -20px;top: 15px;}\n";
  ptr +=".data{padding: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  
   ptr +="<div id=\"webpage\">\n";
   
   ptr +="<h2>MONITOREANDO...</h2>\n";
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side temperature-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n";
   ptr +="width=\"9.915px\" height=\"22px\" viewBox=\"0 0 9.915 22\" enable-background=\"new 0 0 9.915 22\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M3.498,0.53c0.377-0.331,0.877-0.501,1.374-0.527C5.697-0.04,6.522,0.421,6.924,1.142\n";
   ptr +="c0.237,0.399,0.315,0.871,0.311,1.33C7.229,5.856,7.245,9.24,7.227,12.625c1.019,0.539,1.855,1.424,2.301,2.491\n";
   ptr +="c0.491,1.163,0.518,2.514,0.062,3.693c-0.414,1.102-1.24,2.038-2.276,2.594c-1.056,0.583-2.331,0.743-3.501,0.463\n";
   ptr +="c-1.417-0.323-2.659-1.314-3.3-2.617C0.014,18.26-0.115,17.104,0.1,16.022c0.296-1.443,1.274-2.717,2.58-3.394\n";
   ptr +="c0.013-3.44,0-6.881,0.007-10.322C2.674,1.634,2.974,0.955,3.498,0.53z\"/>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<meta http-equiv=\"refresh\" content=\"2\">\n";
   ptr +="<div class=\"side-by-side temperature-text\">Temperatura</div>\n";
   ptr +="<div class=\"side-by-side temperature\">";
   ptr +=(int)temperatura;
   ptr +="<span class=\"superscript\"> C</span></div>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side humidity-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_2\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n\"; width=\"12px\" height=\"17.955px\" viewBox=\"0 0 13 17.955\" enable-background=\"new 0 0 13 17.955\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M1.819,6.217C3.139,4.064,6.5,0,6.5,0s3.363,4.064,4.681,6.217c1.793,2.926,2.133,5.05,1.571,7.057\n";
   ptr +="c-0.438,1.574-2.264,4.681-6.252,4.681c-3.988,0-5.813-3.107-6.252-4.681C-0.313,11.267,0.026,9.143,1.819,6.217\"></path>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side humidity-text\">Humedad</div>\n";
   ptr +="<div class=\"side-by-side humidity\">";
   ptr +=(int)humedad;
   ptr +="<span class=\"superscript\">%</span></div>\n";
  
   ptr +="<h1> Estado del suelo </h1>";
   ptr +="<div class=\"side-by-side humidity-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_2\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n\"; width=\"12px\" height=\"17.955px\" viewBox=\"0 0 13 17.955\" enable-background=\"new 0 0 13 17.955\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M1.819,6.217C3.139,4.064,6.5,0,6.5,0s3.363,4.064,4.681,6.217c1.793,2.926,2.133,5.05,1.571,7.057\n";
   ptr +="c-0.438,1.574-2.264,4.681-6.252,4.681c-3.988,0-5.813-3.107-6.252-4.681C-0.313,11.267,0.026,9.143,1.819,6.217\"></path>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side temperature-text\">Humedad</div>\n";
   ptr +="<div class=\"side-by-side temperature\">";
   ptr +=(int) lectura;
   ptr +="<span class=\"superscript\"> %</span></div>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"data\">\n";
   ptr +="<h3>";
   ptr +=estado;
   ptr +="</h3>";
   ptr +="<span class=\"superscript\"></span></div>\n";
   
   ptr +="</div>\n";
   ptr +="</body>\n";
   ptr +="</html>\n";
   return ptr;
}
