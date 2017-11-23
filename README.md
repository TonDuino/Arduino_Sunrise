# Arduino_Sunrise
Sunrise, Transit and Sunset calculator for any place on Earth at any date, for Arduino

Based on http://aa.quae.nl/en/reken/zonpositie.html

Input:  Longitude in degrees; West is positive, East is negative (coded parameter)
        Latitude in degrees; North is positive, South negative (coded parameter)
        Date: year, Month, monthDay (keyboard input)
        Timezone (coded parameter)
        Day savings offset (coded parameter)
        
Output is a list of the next events:
        1. Time of sunrise, Azimuth (angle with direction South at the horizon)
        2. Time of Solar Transit, when the sun is the highest in the sky, Azimuth and altitude in degrees above the horizon
        3. Time of Solar Noon (per defintion 12:00 UTC), where the sun is in the sky at that moment
        4. Time of sunset, Azimuth
        
Beyond the polar circles, the sun may never set (all day light) or never rise (all day dark). The sketch will detect and report this.

Peculiarities:
- beyond the polar circles the sun may rise after noon, or set before noon;
- beyond the polar circles the sun may rise the day before or set the day after.
