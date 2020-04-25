// This is for dev-testing only.
// Run this as npm run mqtt
// And watch it for published messages as you publish
// to the correct topic from the app.

const mqtt = require('mqtt')
const config = require("./config");
const creds = config["ably-api-key"].split(':');

var client  = mqtt.connect("mqtts://mqtt.ably.io", {
    username: creds[0],
    password: creds[1]
});

const sleep = ms => new Promise(resolve => setTimeout(resolve, ms));

client.on('connect', function () {
  client.subscribe('tshirt', async function (err) {
    if (!err) {
      client.publish('tshirt', 'C'); // connected message      
    }
  })
});

client.on('message', function (topic, message) {
  console.log(topic.toString(), message.toString());
});