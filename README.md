Interactive Lights
=================

**Contents**
* Introduction
* Quickstart
* Teardown
    * The Hardware UI
    * AdaFruit Feather Huzzah ESP8266
    * The Wearable LED Matrix
    * Snake Mode
    * The Web UI
    * Why only specific colours?
    * Making the WebUI and the hardware talk
    * Authenticating our WebUI
    * What's MQTT
    * Ably and MQTT
    * What messages are we sending
    * When are we sending them
    * Keeping the Web UI in sync
    * Auto-Clearing
    * How the hardware works
    * Our main loop
    * Processing messages
* Running the WebUI
* Running the Hardware code
* Configuration


A Node.js webapp, interacting with Ably over the Ably JavaScript SDK, and a hardware counterpart that subscribes to the message over MQTT on an Arduino.
When coloured lights are pressed in the web UI, the t-shirt lights up!

# Quickstart

* NPM install in the repository root
* Crete an Ably account to get an Ably API key - docs here
* Create a .env file with an Ably API key in it

```
    ABLY_API_KEY=your-api-key-here
```
* Open the Arduino sketch file `./hardware/InteractiveLights/InteractiveLights.ino`
* Edit the configuration object at the top of the code
    * Add your Wifi credentials
    * Add your Ably API Key - the two parts of your API key are split on the ":"
    * If your API key is `thisis:yourkey` then the credentials would look like this
    
```c++
    configuration cfg = {
    { "your-wifi-ssid", "your-wifi-password" },
    { "mqtt.ably.io", 8883, "thisis", "yourkey", 
        "3b b1 e6 46 12 f9 f4 ac 53 59 f4 97 99 ee 35 c9 3b 3b 46 11", 
        "tshirt" 
    }
    };
```
* Deploy the Arduino code to your `AdaFruit Feather Huzzah ESP8266`
* Run the `WebUI` by typing `npm run start`
* Browse to the running Web App and paint some pixels!

# Teardown

It's split into two parts.

* `WebUI` - with a colour grid to paint on
* `HarwareUI` some C++ code written for the Arduino compatible hardware we're using.

# The Hardware UI

The hardware UI is made up of two things:

* An AdaFruit Feather Huzzah ESP8266
* A custom built wearable LED Matrix

# AdaFruit Feather Huzzah ESP8266

The AdaFruit Feather Huzzah is a low power, cheap Arduino compatible System-on-chip.

You can run C and C++ code on out of the box. 

It's a hobbiest chip, and you can write software for it using the Arduino IDE.

# The Wearable LED Matrix

The Hardware UI runs on a custom built Wearable LED Matrix. It's got a resolution of 16x16 pixels.

It's built out of NeoPixel strips, cut up and soldiered into a display shape.
NeoPixels 

It's wired together in **GLORIOUS SNAKE MODE**.

# SNAKE MODE

Snake mode is what happens when you optimise for soldering 256 pixels the (moderately, slightly, a little bit more practical) easy way instead of the long boring way.

Our pixels have an odd numbering scheme - the Pixel IDs of the addressable pixels look like this:

    <------------------------<------------------------<------------
    015 014 013 012 011 010 009 008 007 006 005 004 003 002 001 000
    ------------------------->------------------------>------------
    016 017 018 019 020 021 022 023 024 025 026 027 028 029 030 031
    <------------------------<------------------------<------------
    047 046 045 044 043 042 041 040 039 038 037 036 035 034 033 032
    ------------------------->------------------------>------------
    ................

Because of this somewhat unorthadox numbering scheme, the code running on our hardware translates regular, real people pixel Ids, into **GLORIOUS SNAKE IDS** transparently. This means that in our web app, we can address our pixels using the much more reasonable pixel Ids of

    000 001 002 003 004 005 006 007 008 009 010 011 012 013 014 015
    ------------------------->------------------------>------------
    016 017 018 019 020 021 022 023 024 025 026 027 028 029 030 031
    ------------------------->------------------------>------------
    ................


We love snake mode 🐍, it makes soldiering easier. And snakes are awesome. Shut up.

# The Web UI

The Web UI is a HTML and JavaScript app that represents the individual pixels on our Wearable LED Matrix with squares on the screen.

There's a colour picker at the top for people to select their brushes, and a clickable array of squares to paint onto.

# Why only specific colours?

The LED matrix works better with certain colour pallets because painting in light is hard you!
So we picked some safe colours to let people paint with. The pixels themselves are lit using
`RGB` values, but we're constraining our painting to colours we know look good on the hardware.


# Making the WebUI and the hardware talk

We're using Ably's JavaScript SDK in our `WebUI` to send messages to our hardware when a Pixel is clicked.

The cool thing is, all the other users of our `WebUI` are also subscribed to this messages, so their clients can update when other people paint cooperatively with them!

One of the cool features of Ably's `pub/sub` support, is that they support the `MQTT` protocol.

# Authenticating our WebUI

We're using the Ably SDKs `createTokenRequest` feature in our `WebUI` to provide all the front-end clients valid authentication to connect to our `stream`.

```js
async function connect() {
  const ably = new Ably.Realtime.Promise({ authUrl: '/api/createTokenRequest' });
  const channelId = "tshirt";
  const channel = await ably.channels.get(channelId);
  await channel.attach();
  ...
}
```

This requires just a couple of lines of backend code on our `Node.js` server and an Ably API key.

```js
const client = new Ably.Realtime(config["ably-api-key"]);

app.get("/api/createTokenRequest", async (request, response) => {
  const tokenRequestData = await client.auth.createTokenRequest({ clientId: 'interactive-lights-ui' });
  response.send(tokenRequestData);
});
```

# What's MQTT

From MQTT.org

    MQTT is a machine-to-machine (M2M)/"Internet of Things" connectivity protocol. It was designed as an extremely lightweight publish/subscribe messaging transport. It is useful for connections with remote locations where a small code footprint is required and/or network bandwidth is at a premium.

# Ably and MQTT

So the cool thing about Ably supporting MQTT is that any messages we send in our browser or our `Node.js` process using the `JavaScript` SDK, are also sent out over the MQTT protocol automatically.

This means that we can use a lightweight MQTT library on our Feather Huzzah, and a simpler, more standard JavaScript / HTTP SDK in our browser.

MQTT is designed to be fast and responsive, so we can communicate between multiple devices and our clients with really low latency and low bandwidth requirements.

# What messages are we sending

| Message Type      | Format                                                                | Example                 |
|-------------------|-----------------------------------------------------------------------|-------------------------|
| Connected         | A single letter C                                                     | C                       |
| Sync              | S: Prefixed, CSV of Pixel messages for set pixels, trailing comma     | S:100#FFFFCC,1#CCCCCC.. |
| Pixel             | A pixel number, followed by a hex colour                              | 100#FFFFFF              |
| Clear             | A single letter X                                                     | X                       |


# When are we sending them

The MQTT message exchanges follow the followng rules:

* When a `WebUI` subscribes, they should send a `Connected message`.
* When a `Connected message` is received, `HardwareUi` should send entire current state for `WebUI` - a `Sync message`.
* When a `Sync message` is sent from the `HardwareUi`, `HardwareUi` should ignore it.
* When a `Sync message` is received, `WebUI` should update it's current visual state.
* When a `Pixel message` is received, `HardwareUi` should update corresponding light.
* When a `Pixel message` is received, `WebUI` should update corresponding light.
* When a `Pixel message` is received, `HardwareUi` should keep a copy in memory of that locations light colour for `Sync message`.
* When a `Clear message` is sent, the `WebUI` should reset
* When a `Clear message` is sent, the `HardwareUI` should reset


# Keeping the Web UI in sync

Because we want everyone to be painting together, we want to keep all our UIs in sync.

We do this in two ways

**Sync messages**
Whenever a client subscribes to our events, it sends a `Connected` message - the single letter C.
When our hardware sees that message, it'll send out a `Sync message` - a CSV of all the lit pixels, and their colours.

The clients can then update their own state to reflect the current state of the hardware.

**Subscribing to Pixel messages**
Because everything is subscribed to the same Ably channel we can update all the WebUIs whenever a `Pixel message` is seen.

We do this all in the JavaScript code in `public/script.js` in our repo.

```js

function processMessage(message) {
  const data = message.data;
  const value = data.constructor.name == "ArrayBuffer" 
                  ? new TextDecoder("utf-8").decode(data) 
                  : data;
  
  switch(value[0]) {
    case "C":
      break;    
    case "X":
      resetDisplay();
      break;
    case "S":
      processSyncMessage(value);
      break;
    default:
      syncSinglePixel(value);
      break;
  } 
}

async function connect() {
  const ably = new Ably.Realtime.Promise({ authUrl: '/api/createTokenRequest' });
  const channelId = "tshirt";
  const channel = await ably.channels.get(channelId);
  await channel.attach();
  
  for (let cell of square) {
    cell.addEventListener("click", e => colorAndPublish(e, channel), false);   
  }

  channel.subscribe(processMessage);  
  channel.publish("tshirt", "C");  
  console.log("Subscribed");
}
```

# Auto-Clearing

In the spirit of magic whiteboards everywhere, the display auto-clears itself every 20 seconds to let more people enjoy painting pretty colours!

This is all controlled in the main `loop()` function of the Arduino code, and can be disabled by removing the calls to 

```c++
    processor->clear_state();
    snake_lights::device_ready_light();
```

This feature is disabled by default, uncomment it in the .ino file to enable it.

# How the hardware works

Arduino sketches are split into two parts

* void setup() { ... }
* void loop() { ... }

Setup is run exactly once, and the loop function is called continiously by the hardware. You then delay for as long as you like at the end of your function, to provide a pause before it's run again.

# Our main loop

After our setup takes care of enabling our lights, and connecting to wifi, our main loop in `InteractiveLights.ino` looks like this:

```c++

void loop()
{    
  if (ticks == 80) {
    Serial.println("🚫🐓");
    ticks = 0;
    processor->clear_state();
    snake_lights::device_ready_light();
  }

  networking::ensure_wifi_connected(cfg.wifi.ssid, cfg.wifi.password);
  mqtt->ensure_mqtt_connected(process);  
  mqtt->process_messages();

  delay(250);
  ticks++;
}
```

Our device is "ticking", executing this loop, every 250ms. Every 80th tick, we reset our display to clear our pixels down.

We then call three things

* ensure_wifi_connected - makes sure our connection hasn't dropped
* ensure_mqtt_connected - makes sure we're still listening to the MQTT stream
* process_messages - calls our `process` function for every message that has arrived since the last tick

We then pause and go again.

# Processing messages

All our message processing code is in `MessageProcessor.cpp`

It's outline looks something like this:

```c++

void message_processor::process(String& topic, String& framedata)
{ 
  if (framedata == Empty) return;
  if (framedata[0] == ClearMessage) return;
  if (framedata[0] == SyncMessagePrefix) return;
  if (framedata == ConnectedMessage)
  {    
    // send sync message
    ...
    return;
  }
  
  // snake_lights::update_pixel
  ....
}
```
This function is called every time a message arrives on the hardware.


# Running the Web UI

First, make sure you `npm install`.

Browsing to the root of the webserver on http://localhost:12271 will take you to the WebUI.

You can start the app by running `npm start`.

    > npm start
    
    > hello-express@0.0.1 start C:\dev\interactive-lights
    > node server.js
    
    Your app is listening on port 12271

If you want to develop against this with hot-reload and debugging enabled, first, make sure you've installed nodemon

    > npm install -g nodemon
    
then you can simply run `npm run devserver`

    > npm run devserver

    > hello-express@0.0.1 devserver C:\dev\interactive-lights
    > nodemon --inspect server.js

    [nodemon] 1.19.4
    [nodemon] to restart at any time, enter `rs`
    [nodemon] watching dir(s): *.*
    [nodemon] watching extensions: js,mjs,json
    [nodemon] starting `node --inspect server.js`
    Debugger listening on ws://127.0.0.1:9229/746db334-5730-44d5-8764-e68913a53be3
    For help, see: https://nodejs.org/en/docs/inspector
    Your app is listening on port 12271


This will run the server on the same port, with --inspect enabled to connect `VSCode` or `WebStorm` debuggers to step through the code.
It'll also hot-reload any javascript changes, so you don't have to cycle your node process during dev.

* For VSCode see: https://code.visualstudio.com/docs/nodejs/nodejs-debugging
* For WebStorm see: https://www.jetbrains.com/help/webstorm/running-and-debugging-node-js.html


# Running the hardware

There's an Arduino sketch in `/hardware/InteractiveLights/InteractiveLights.ino`.

Arduino requirements:
- ESP8266 hardware - tested on an AdaFruit Feather Huzzah
- Arduino MQTT library (downloadable from Library Manager)
    - https://github.com/256dpi/arduino-mqtt
- NeoPixels library (bundled in this repo).

- Update the WiFi credentials with your own
- Add your own API key
- Build the .ino sketch and push it over USB to your hardware.

- You will need to make your own wearable LED matrix!


## Configuration

### For Node 

Create a .env file in the root of your repository with your Ably API key in it

```
    ABLY_API_KEY=your-api-key-here
```

### For Arduino
* Open the Arduino sketch file `./hardware/InteractiveLights/InteractiveLights.ino`
* Edit the configuration object at the top of the code
    * Add your Wifi credentials
    * Add your Ably API Key - the two parts of your API key are split on the ":"
    * If your API key is `thisis:yourkey` then the credentials would look like this
    
```c++
    configuration cfg = {
    { "your-wifi-ssid", "your-wifi-password" },
    { "mqtt.ably.io", 8883, "thisis", "yourkey", 
        "3b b1 e6 46 12 f9 f4 ac 53 59 f4 97 99 ee 35 c9 3b 3b 46 11", 
        "tshirt" 
    }
    };