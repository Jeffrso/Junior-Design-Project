import asyncio
import sys

import websockets

CLASS_SERVER = "ws://10.5.9.24:80/ws"
LOGIN_ID = "BITBANGER123" # approved client ID (same as the robot)
TO_BOT2 = "BITBANGER:MOVE:1" # Bot 1 -> Bot 2: our robot drives when it sees this
TO_BOT1 = "DJKP2" # Bot 2 -> Bot 1: the partner robot drives when it sees this
MOVE_S = 5

busy = False

async def drive_then_reply(ws, role, reply):
    global busy
    print(f"[{role}] LED blinking, driving forward {MOVE_S} s...")
    await asyncio.sleep(MOVE_S)
    print(f"[{role}] stopped")
    if reply:
        print(f"[{role}] LED flash, sending {reply!r}")
        await ws.send(reply)
    busy = False

async def start_on_enter(ws):
    loop = asyncio.get_running_loop()
    while True:
        line = await loop.run_in_executor(None, sys.stdin.readline)
        if not line:
            return
        print(f"[bot1] LED flash, sending {TO_BOT2!r}")
        await ws.send(TO_BOT2)

async def main(role, server):
    global busy
    listen_for = TO_BOT2 if role == "bot2" else TO_BOT1
    reply = TO_BOT1 if role == "bot2" else None
    async with websockets.connect(server) as ws:
        await ws.send(LOGIN_ID)  # first message must be an approved client ID
        print(f"[{role}] connected to {server}, waiting for {listen_for!r}")
        if role == "bot1":
            print("[bot1] press Enter to start the demo")
            asyncio.create_task(start_on_enter(ws))
        async for msg in ws:
            print(f"    server: {msg}")
            if listen_for in msg:
                if busy:
                    print(f"[{role}] busy: ignored")
                else:
                    busy = True
                    asyncio.create_task(drive_then_reply(ws, role, reply))

if __name__ == "__main__":
    if len(sys.argv) < 2 or sys.argv[1] not in ("bot1", "bot2"):
        print("usage: python tools/partner_test.py bot1|bot2 [ws://server:port/ws]")
        sys.exit(1)
    server = sys.argv[2] if len(sys.argv) > 2 else CLASS_SERVER
    try:
        asyncio.run(main(sys.argv[1], server))
    except KeyboardInterrupt:
        pass
