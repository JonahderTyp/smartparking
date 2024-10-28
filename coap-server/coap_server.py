import asyncio

from aiocoap import Context, Message, resource

# Global variable to store the latest status of the parking bays
latest_parkhaus_data = None


class ParkhausResource(resource.Resource):
    async def render_post(self, request: Message):
        global latest_parkhaus_data
        # Expect a comma-separated string, e.g., "1,0,1,1" for bay statuses
        latest_parkhaus_data = request.payload.decode('utf-8')
        print("Received Parkhaus data:", latest_parkhaus_data)
        return Message(payload=b"Data received by CoAP server")


async def main():
    root = resource.Site()
    root.add_resource(['parkhaus'], ParkhausResource())
    server_context = await Context.create_server_context(root)
    print("CoAP Server running...")
    # Keep server running indefinitely
    await asyncio.get_running_loop().create_future()

if __name__ == "__main__":
    asyncio.run(main())
