import asyncio

from aiocoap import *


async def get_booleans():
    context = await Context.create_client_context()

    request = Message(code=GET, uri="coap://localhost/boolean")
    response = await context.request(request).response

    print("Received:")
    print(response.payload.decode("utf-8"))

if __name__ == "__main__":
    asyncio.run(get_booleans())
