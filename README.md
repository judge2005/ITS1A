# ITS1A Clock Code

This code is specifically for my ITS1A clock, which is documented here: https://www.nixies.us/projects/its1a-clock/

The current version uses NTP for time synchronization and can handle daylight-savings time. The format is defined [here](https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html). There is a full list of strings defined [here](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv).

Here are some examples:

| Location | Value |
| ------------- | ------------- |
| US Central | CST6CDT,M3.2.0,M11.1.0 |
| US Eastern | EST5EDT,M3.2.0,M11.1.0 |
| US Mountain | MST7MDT,M3.2.0,M11.1.0 |
| US Pacific | PST8PDT,M3.2.0,M11.1.0 |
| UK | GMT0BST,M3.5.0/1,M10.5.0 |
| Ireland | IST-1GMT0,M10.5.0,M3.5.0/1 |
| Central Europe | CET-1CEST,M3.5.0,M10.5.0/3 |
| Eastern Europe | EET-2EEST,M3.5.0/3,M10.5.0/4 |
| Japan | JST-9 |
| Australia/Brisbane | AEST-10 |
| Australia/Perth | AWST-8 |
| Australia/Adelaide | ACST-9:30ACDT,M10.1.0,M4.1.0/3 |
| Australia/Sydney | AEST-10AEDT,M10.1.0,M4.1.0/3 |
## Compilation From Source
See the source ITS1A.ino for location of dependencies. This is also compiled with the following flags:
    -DNEW_WSTRING -DUSE_EADNS -DESP8266_2_7_3 -DITS1A