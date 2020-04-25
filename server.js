const config = require("./config");
const express = require("express");
const Ably = require('ably/promises');
const client = new Ably.Realtime(config["ably-api-key"]);

const app = express();
app.use(express.static('public'))

app.get("/", async (request, response) => {
  response.sendFile(__dirname + '/views/index.html');
});

app.get("/moderate", async (request, response) => {
  response.sendFile(__dirname + '/views/moderate.html');
});

app.get("/api/createTokenRequest", async (request, response) => {
  const tokenRequestData = await client.auth.createTokenRequest({ clientId: 'interactive-lights-ui' });
  response.send(tokenRequestData);
});

const port = process.env.PORT || 12271;
const listener = app.listen(port, () => {
  console.log("Your app is listening on port " + listener.address().port);
});