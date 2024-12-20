from PIL import Image

def extract_bmp_part(input_file, output_file, x, y, width, height):
    # Open the BMP file
    img = Image.open(input_file)

    # Crop the image to extract the specified part
    cropped_img = img.crop((x, y, x + width, y + height))

    # Save the cropped image
    cropped_img.save(output_file)

if __name__ == "__main__":
    # input_file = "alldigits.bmp"
    input_file = "HashOne.bmp"
    output_file = "output.bmp"

    # Define the coordinates and dimensions for the part to be extracted
    x = 0
    y = 48
    width = 48
    height = 48

    # Extract a part of the BMP file
    for i in range(0,12,2):
        # output_file = "output" + str(i) + ".bmp"
        output_file = "HashOne" + str(i) + ".bmp"
        extract_bmp_part(input_file, output_file, x, int(i * height/10) , width, height)

    print("Part extracted and saved to output.bmp")
