# ESP8266 Based Spotify Controller

This project is built with an ESP8266, ASP.NET Core and React.

### ESP8266

The ESP8266 is the hardware component of the project, it captures button inputs and sends a POST request to the ASP.NET Core backend.

The ESP8266 is programmed with the ESP8266_RTOS_SDK.

### ASP.Net Core

ASP.Net Core is used as the backend of the project. It converts the button presses from the ESP8266 into a POST request to the Spotify API.

While I could directly interact with the Spotify API through the ESP8266, I wanted to easily change the mapping of the buttons. Having a web interface makes this simple.

### React

React is the front-end interface for the project. The interface allows me to remap buttons from the ESP8266 or disable them completely. It also allows me to set the Spotify API key instead of having it hardcoded in the backend.

### Current Progress

I am able to get input from three push buttons and use them to light up an led. Next is to get the ESP8266 connected to the internet.
