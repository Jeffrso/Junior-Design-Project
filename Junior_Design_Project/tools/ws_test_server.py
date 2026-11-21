import argparse
import asyncio
import json
import socket
import sys

import websockets

robots = {} # connection -> client ID it logged in with

async def handler(ws):
    ip = ws.remote_address[0]
    print(f"robot connected from {ip}")
    try:
        async for msg in ws:
            if ws not in robots:
                # first message is client ID, like on the class server
                robots[ws] = msg
                print(f"robot ID: {msg!r} -> sending authenticated ok")
                await ws.send(json.dumps({"type": "authenticated", "status": "ok"}))
            else:
                print(f"{robots[ws]} says: {msg!r}")
                # like class server: every client gets "<ID> <message>"
                websockets.broadcast(robots, f"{robots[ws]} {msg}")
    finally:
        robots.pop(ws, None)
        print(f"robot {ip} disconnected")

def send_ping(text="ping"):
    if not robots:
        print("(no authenticated robot connected yet)")
        return
    websockets.broadcast(robots, text)
    print(f"sent {text!r}")

async def auto_ping(seconds):
    while True:
        await asyncio.sleep(seconds)
        send_ping()

def local_ip():
    # doesn't send anything, just finds which network interface would be used
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(("8.8.8.8", 80))
        return s.getsockname()[0]
    except OSError:
        return "127.0.0.1"
    finally:
        s.close()

async def main(port, every):
    async with websockets.serve(handler, "0.0.0.0", port):
        print(f"listening on ws://{local_ip()}:{port}/ws")
        print(f"  -> in web.cpp set SERVER_IP = \"{local_ip()}\" and SERVER_PORT = {port}")
        print("press Enter to send a ping, Ctrl+C to quit")
        if every:
            asyncio.create_task(auto_ping(every))
        loop = asyncio.get_running_loop()
        while True:
            line = await loop.run_in_executor(None, sys.stdin.readline)
            if not line:
                break
            send_ping(line.strip() or "ping")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, default=8765)
    parser.add_argument("--every", type=float, default=0, help="also send a ping every N seconds")
    args = parser.parse_args()
    try:
        asyncio.run(main(args.port, args.every))
    except KeyboardInterrupt:
        pass
