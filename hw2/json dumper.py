import json
import os

input_folder = "C:/Users/mayal/Desktop/NTUST/Embedded/hw2/hw2-esd-export/testing"
output_folder = "C:/Users/mayal/Desktop/NTUST/Embedded/hw2/output/testing"

os.makedirs(output_folder, exist_ok=True)

print("Files in input folder:", os.listdir(input_folder))

for filename in os.listdir(input_folder):
    if filename.endswith(".json"):
        input_path = os.path.join(input_folder, filename)
        output_path = os.path.join(output_folder, filename)

        print(f"Processing file: {filename}")

        with open(input_path, "r") as json_file:
            data = json.load(json_file)

        print(f"Content of {filename}: {json.dumps(data, indent=4)}")

        # Correct the key to match the actual structure
        sensors = data.get("payload", {}).get("sensors", [])
        print(f"Sensors in {filename}: {sensors}")

        for sensor in sensors:
            print(f"Checking sensor: {sensor}")
            if sensor.get("name") == "light":
                print(f"Changing {sensor['name']} to 'Illumination'")
                sensor["name"] = "Illumination"

        with open(output_path, "w") as json_file:
            json.dump(data, json_file, indent=4)

        print(f"Processed and saved: {filename}")

print(f"All JSON files have been updated and saved to '{output_folder}'.")
