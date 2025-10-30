#!/usr/bin/env python3
"""
Local web server for testing iOS app distribution
Serves the distribution files for testing on iOS devices
"""

import http.server
import socketserver
import os
import sys
import webbrowser
from urllib.parse import urlparse

class iOSDistributionHandler(http.server.SimpleHTTPRequestHandler):
    """Custom handler for iOS app distribution"""
    
    def end_headers(self):
        # Add CORS headers for iOS
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.send_header('Cache-Control', 'no-cache, no-store, must-revalidate')
        self.send_header('Pragma', 'no-cache')
        self.send_header('Expires', '0')
        super().end_headers()
    
    def do_GET(self):
        # Handle different file types
        if self.path == '/':
            self.path = '/distribution.html'
        elif self.path.endswith('.ipa'):
            self.send_header('Content-Type', 'application/octet-stream')
            self.send_header('Content-Disposition', 'attachment; filename="RacingGame3DiOS.ipa"')
        elif self.path.endswith('.plist'):
            self.send_header('Content-Type', 'application/xml')
        
        return super().do_GET()
    
    def log_message(self, format, *args):
        """Custom log format"""
        print(f"[{self.address_string()}] {format % args}")

def get_local_ip():
    """Get the local IP address"""
    import socket
    try:
        # Connect to a remote server to get local IP
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        local_ip = s.getsockname()[0]
        s.close()
        return local_ip
    except:
        return "localhost"

def main():
    """Start the distribution server"""
    PORT = 8000
    
    # Change to the distribution directory
    dist_dir = "distribution"
    if not os.path.exists(dist_dir):
        print(f"Error: Distribution directory '{dist_dir}' not found")
        print("Run './build_distribution.sh' first to create distribution files")
        sys.exit(1)
    
    os.chdir(dist_dir)
    
    # Start server
    with socketserver.TCPServer(("", PORT), iOSDistributionHandler) as httpd:
        local_ip = get_local_ip()
        
        print("=" * 60)
        print("iOS App Distribution Server")
        print("=" * 60)
        print(f"Server running on:")
        print(f"  Local:   http://localhost:{PORT}")
        print(f"  Network: http://{local_ip}:{PORT}")
        print()
        print("To install on iOS device:")
        print(f"1. Connect iOS device to same WiFi network")
        print(f"2. Open Safari on iOS device")
        print(f"3. Go to: http://{local_ip}:{PORT}")
        print("4. Tap 'Install on iOS' button")
        print()
        print("Files being served:")
        for file in os.listdir("."):
            if os.path.isfile(file):
                size = os.path.getsize(file)
                print(f"  {file} ({size:,} bytes)")
        print()
        print("Press Ctrl+C to stop server")
        print("=" * 60)
        
        # Open browser on local machine
        try:
            webbrowser.open(f"http://localhost:{PORT}")
        except:
            pass
        
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nServer stopped.")
            sys.exit(0)

if __name__ == "__main__":
    main()