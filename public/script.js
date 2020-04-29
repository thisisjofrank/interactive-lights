console.log("Oh hai! 🖤");

const colors = document.getElementsByClassName('color');
const squares = document.getElementsByClassName('square');
const colorMap = [...document.querySelectorAll('[data-rgb]')];

let selected = "white";
let rgb = "#FFFFFF"

function setActiveColor(e) {
  selected = e.target.id;
  rgb = e.target.dataset.rgb;
  console.log(rgb);
}
  
for (let color of colors) {
  color.addEventListener("click", setActiveColor, false);   
}

function colorAndPublish(e, channel) {
  let pixel = e.target.id.replace('square', '');
  e.target.style.fill = selected;  
  channel.publish("tshirt", pixel + rgb);  
}

function syncSinglePixel(compoundValue) {
  const parts = compoundValue.split('#');
  const pixelNumber = parts[0];
  const pixelColor = ("#" + parts[1]).toLowerCase();

  const id = "square" + pixelNumber.padStart(3, '0');
  const colourToFillWith = colorMap.filter(entry => entry.dataset.rgb == pixelColor);

  if (colourToFillWith.length > 0) {
    const cssName = colourToFillWith[0].id;    
    document.getElementById(id).style.fill = cssName;
  } else {      
    console.log("Couldn't find the colour requested.");
  }
}

function processSyncMessage(message) {
  const pixelValues = message.slice(2).split(',');
  pixelValues.pop();  
  for (var compoundValue of pixelValues) {
    syncSinglePixel(compoundValue);
  }
}

function resetDisplay() {
  for (let color of square) {
    color.style.fill = "black";
  }
}

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
  
  const onClick = (e) => colorAndPublish(e, channel);
  for (let square of squares) {
    square.addEventListener("click", onClick, false);   
  }

  channel.subscribe(processMessage);  
  channel.publish("tshirt", "C");  
  console.log("Subscribed");
  
  const moderation = document.getElementById('clear');
  if (moderation) {
    for (let square of squares) {
      square.removeEventListener("click", onClick, false);   
    }
    moderation.addEventListener("click", e => {      
      channel.publish("tshirt", "X");  
    });
  }
}

connect(); 