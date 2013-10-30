General information about APIs
==============================

Discogs
-------
 - https://www.discogs.com/developers/accessing.html
 - User Agent: [RFC 1945](http://tools.ietf.org/html/rfc1945#section-3.7)
 - Rate limits:
   - 1 request per second per ip address
   - 1000 images requests per day per ip address
 - Returns HTTP response codes to requests to inform about errors
 - JSON format, pretty simple

```json
{
  "id": 1,
  "name": "Persuader, The"
}
```

Spotify
-------
 - https://developer.spotify.com/technologies/web-api/
 - Authorization required
 - Terms are not clearly defined
   - API access can be removed without notification
   - The API should not be used intensively, but the rate is not well defined
 - Must display a disclamer ("This product uses a SPOTIFY API but is not endorsed, certified or otherwise approved in any way by Spotify. Spotify is the registered trade mark of the Spotify Group.")
 - JSON format, not many interesting information

```json
{
    "info": {
        "limit": 100,
        "num_results": 2,
        "offset": 0,
        "page": 1,
        "query": "333 d-nox beckers",
        "type": "track"
    },
    "tracks": [
        {
            "album": {
                "availability": {
                    "territories": "AD AR AT AU BE CA CH DE DK EE ES FI FR GB GR HK IE IS IT LI LT LU LV MC MX MY NL NO NZ PL PT SE SG TR TW US"
                },
                "href": "spotify:album:3Qb8xVeIh0d99oEhujKgOa",
                "name": "Sunrise Festival 2010",
                "released": "2010"
            },
            "artists": [
                {
                    "href": "spotify:artist:0HJC42HKgBC0gk1DnnY3Z1",
                    "name": "FM Radio Gods"
                }
            ],
            "external-ids": [
                {
                    "id": "GBKFC1011042",
                    "type": "isrc"
                }
            ],
            "href": "spotify:track:1HlM8zKRvV3apUCgqLIQUf",
            "length": 462.45100000000002,
            "name": "333 - D-Nox & Beckers Remix",
            "popularity": "0.14",
            "track-number": "7"
        },
        {
            "album": {
                "availability": {
                    "territories": "AD AR AT AU BE CA CH DE DK EE ES FI FR GB GR HK IE IS IT LI LT LU LV MC MX MY NL NO NZ PL PT SE SG TR TW US"
                },
                "href": "spotify:album:0B5IOsHYhcdimnDdttLB6A",
                "name": "Digital Progression",
                "released": "2010"
            },
            "artists": [
                {
                    "href": "spotify:artist:0HJC42HKgBC0gk1DnnY3Z1",
                    "name": "FM Radio Gods"
                }
            ],
            "external-ids": [
                {
                    "id": "GBKFC1011042",
                    "type": "isrc"
                }
            ],
            "href": "spotify:track:3ZIviAPg5XEoUhz8PtXp52",
            "length": 462.45100000000002,
            "name": "333 (D-Nox & Beckers Remix)",
            "popularity": "0.01",
            "track-number": "9"
        }
    ]
}
```

GraceNote
---------
 - https://developer.gracenote.com/web-api
 - Free for non-commercial use

Beatport
--------
 - Authorization required
 - OAuth mandatory
 - Must display "Buy this track on Beatport" link
 - Must display a disclamer ("THIS APPLICATION IS NOT AFFILIATED WITH, MAINTAINED, ENDORSED OR SPONSORED BY BEATPORT, LLC OR ANY OF ITS AFFILIATES.")
 - Non commercial usage

Juno
----
 - Terms and conditions not really clear
 - Is it possible to use their API whithout paying for a non-commercial project ?

Deezer
------
 - http://developers.deezer.com/api
 - Terms are not clearly defined
   - API access can be removed without notification
   - The API should not be used intensively, but the rate is not well defined
 - Multi-artists are not separated in responses

MusicBrainz
-----------
 - http://wiki.musicbrainz.org/Development/XML_Web_Service/Version_2
 - Terms of use relatively open
 - XML responses
 - Catalog incomplete

MusicStory
----------
 - http://developers.music-story.com
 - Free for non-commercial use

OneMusicAPI
-----------
 - Aggregate many catalogs (3.000.000 albums)
 - 1000 lookups only, must pay for more
