from groq import Groq
import socket
import os
from dotenv import load_dotenv

load_dotenv()

host = "127.0.0.1"
port = 9090

serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

serverSocket.bind((host, port))
serverSocket.listen(1)
print(f"LLM Server listening on {host} and {port}")

while True:
    incomingConnection, address = serverSocket.accept()
    print(f"Connection from {address} has been established!")

    try:
        while True:
            message = incomingConnection.recv(1024).decode()
            if not message:
                break
            
            print(f"Message from client: {message}")

            api_key = os.getenv('GROQ_API_KEY')
            if not api_key:
                raise ValueError("GROQ_API_KEY environment variable not set")

            client = Groq(api_key=api_key)
            chat_completion = client.chat.completions.create(
                messages=[
                    {"role": "system", "content": "you are a helpful assistant."},
                    {"role": "user", "content": message},
                ],
                model="llama3-8b-8192",
                temperature=0.5,
                max_tokens=1024,
                top_p=1,
                stop=None,
                stream=False,
            )

            response = chat_completion.choices[0].message.content.encode()
            incomingConnection.send(response)
    
    except ValueError as ve:
        print(f"ValueError: {str(ve)}")
        incomingConnection.send(b"ValueError: GROQ_API_KEY environment variable not set")
    
    except Exception as e:
        print(f"Error occurred: {str(e)}")
        incomingConnection.send(b"Error occurred during message processing")
    
    finally:
        incomingConnection.close()

serverSocket.close()
