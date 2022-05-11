## Troubleshooting for disconnected gateway

When you see on TTN that the gateway is disconnected this can have varying reasons and solutions.
If you have easy physical access to the gateway:

- Make sure the Gateway has power and is powered on.
- Make sure the Gateway has internet access.

More than not, the gateway might just need a reboot. Maybe it didn't reboot properly in the first place, or there might be a network error.
To check remotely:

1. Open a console and try connecting to the Gateway with SSH.
	>> If this does not work, make sure you have the correct IP-address. Else you have to physically check the gateway.
	>> See above guide how to find the IP-address and connect with SSH.
2. Once connected use `sudo reboot` to restart the Gateway. Wait a little while and connect again.
3. Now check the log file to see if Basic Station is working: `tail -f TTN/station.log`
	>> See the Troubleshooting if you get any errors here.
4. If everything works the Gateway should show as Connected again on TTN. If not you will need physical access to troubleshoot further.
5. If nothing else seems to work you could also try restoring a backup to a new SD-card and see if that works. (It did once for us.)


---