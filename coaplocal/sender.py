import asyncio

from aiocoap import *


async def send_boolean(key, value):
    context = await Context.create_client_context()
    payload = f"{key}={value}".encode("utf-8")

    request = Message(code=PUT, payload=payload,
                      uri="coap://localhost/boolean")
    response = await context.request(request).response

    print(f"Sent: {key}={value}")
    print(f"Response: {response.code} - {response.payload.decode('utf-8')}")

if __name__ == "__main__":
    asyncio.run(send_boolean("value1", "true"))
    asyncio.run(send_boolean("value2", "false"))
    asyncio.run(send_boolean("value3", "true"))
    asyncio.run(send_boolean("value4", "false"))
