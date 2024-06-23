# llmGeneratedChatApplication

## Compile cpp files
```
  g++ server.cpp -o build/server
  g++ client.cpp -o build/client
```

## Create .env file in root folder
```
.env.example
  GROQ_API_KEY=YOUR_API_KEY
  HOST=YOUR_HOSTED_IP
  PORT=YOUR_PORT_NUMBER
```

## Execute
```
  python3 llmResponse.py
  ./build/server
  ./build/client
```
