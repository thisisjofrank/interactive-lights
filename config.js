require('dotenv').config();

const apiKey = process.env.ABLY_API_KEY;

if(!apiKey) {
    throw "Your process.env.ABLY_API_KEY is empty. Is there a .env file in the repository root?";
}

module.exports = {
    "ably-api-key": process.env.ABLY_API_KEY
};
