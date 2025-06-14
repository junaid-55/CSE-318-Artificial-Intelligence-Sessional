# bash Engine/run.sh
# python ui/test.py

#!/bin/bash
# setup_vnc.sh

# Install packages
sudo apt-get update
sudo apt-get install -y xfce4 xfce4-goodies tightvncserver novnc websockify

# Set up VNC
export DISPLAY=:1
echo "Setting up VNC server..."

# Start VNC server
vncserver :1 -geometry 1280x720 -depth 24

# Start noVNC
cd /tmp
git clone https://github.com/novnc/noVNC.git
cd noVNC
./utils/launch.sh --vnc localhost:5901 --listen 6080 &

echo "VNC setup complete!"
echo "1. Forward port 6080 in Codespaces"sxs
echo "2. Access via the forwarded URL"
echo "3. Run: export DISPLAY=:1 && python main.py"