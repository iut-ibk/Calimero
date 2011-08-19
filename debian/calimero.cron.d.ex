#
# Regular cron jobs for the calimero package
#
0 4	* * *	root	[ -x /usr/bin/calimero_maintenance ] && /usr/bin/calimero_maintenance
