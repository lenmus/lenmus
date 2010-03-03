#
# Regular cron jobs for the lenmus package
#
0 4	* * *	root	[ -x /usr/bin/lenmus_maintenance ] && /usr/bin/lenmus_maintenance
