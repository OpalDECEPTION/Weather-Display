import board
import busio
import requests
from adafruit_ht16k33 import segments
import neopixel

pixel_pin = board.D18  # GPIO 16
num_pixels = 8
pixels = neopixel.NeoPixel(pixel_pin, num_pixels, brightness=0.5, auto_write=True)

api_key = "85c8420c016866562ddf5502749ac681"
city_name = "Boston"
url = "http://api.openweathermap.org/data/2.5/weather"
params = {"q": city_name, "appid": api_key, "units": "imperial"}


# Set up I2C interface
i2c = busio.I2C(board.SCL, board.SDA)

# Create display instance
display = segments.Seg7x4(i2c)

# Set brightness (0.0 to 1.0)
display.brightness = 0.7



try:
    response = requests.get(url, params=params, timeout=10)
    response.raise_for_status()  # raise for HTTP errors
    data = response.json()
except requests.exceptions.RequestException as e:
    print("Error fetching data:", e)
else:
    try:
        temp_f = data["main"]["temp"]
        description = data["weather"][0]["main"]  # e.g., "Rain", "Clear", etc.

    # Round temp and convert to string
        temp_str = str(int(round(temp_f)))

    # Prepend "R" if it's rainy
        if description.lower() == "rain":
            display_str = f"{temp_str:>3}*F"  
            for i in range(8):
                pixels[i] = (0, 102, 204)  # RGB: Blue
        if description.lower() == "clouds":
            display_str = f"{temp_str:>3}*F"  
            for i in range(8):
                pixels[i] = (128, 128, 128)  # RGB: Gray
        if description.lower() == "clear":
            display_str = f"{temp_str:>3}*F"  
            for i in range(8):
                pixels[i] = (102, 204, 255)  # RGB: Sky Blue
        if description.lower() == "snow":
            display_str = f"{temp_str:>3}*F"  
            for i in range(8):
                pixels[i] = (255, 255, 255)  # RGB: White
        if description.lower() == "drizzle":
            display_str = f"{temp_str:>3}*F"  
            for i in range(8):
                pixels[i] = (81, 117, 135)  # RGB: Gray/Blue
        if description.lower() == "thunderstorm":
            display_str = f"{temp_str:>3}*F"  
            for i in range(8):
                pixels[i] = (255, 255, 0)  # RGB: Yellow

        display.print(display_str)
        print(f"Sky: {description.capitalize()}")

    except (KeyError, IndexError) as e:
        print("Unexpected response structure:", e)
        print("Full response:", data)