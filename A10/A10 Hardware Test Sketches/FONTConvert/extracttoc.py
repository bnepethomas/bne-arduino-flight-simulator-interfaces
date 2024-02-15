from PIL import Image

def extract_bmp_part(input_file, output_file, x, y, width, height):
    # Open the BMP file
    img = Image.open(input_file)

    # Crop the image to extract the specified part
    cropped_img = img.crop((x, y, x + width, y + height))

    # Save the cropped image
    cropped_img.save(output_file)

def export_to_c_format(image_data, width, height, output_file):
    with open(output_file, 'w') as f:
        f.write('#include <stdint.h>\n\n')
        f.write('const uint8_t image_data[] = {\n')

        # Thiscode is mapping a pixel to a byte - needs to map 8 pixels to a byte
        for y in range(height):
            for x in range(0,width,8):
                print(x)
                pixel_0 = image_data[y * width + x]
                pixel_1 = image_data[y * width + x + 1]
                pixel_2 = image_data[y * width + x + 2]
                pixel_3 = image_data[y * width + x + 3]
                pixel_4 = image_data[y * width + x + 4]
                pixel_5 = image_data[y * width + x + 5]
                pixel_6 = image_data[y * width + x + 6]
                pixel_7 = image_data[y * width + x + 7]

                pixel = pixel_7 * 128 + pixel_6 * 64 + pixel_5 * 32 + pixel_4 * 16 + pixel_3 * 8 + pixel_2 * 4 + pixel_1 * 2 + pixel_0
                
                f.write(f'0x{pixel:02X}, ')
            f.write('\n')

        f.write('};\n')
        f.write(f'const int image_width = {width};\n')
        f.write(f'const int image_height = {height};\n')

if __name__ == "__main__":
    input_file = "48-48-2.bmp"
    output_file = "output.bmp"
    cropped_output_file = "cropped_output.bmp"
    c_output_file = "image_data.c"

    # Extract a part of the BMP file
    extract_bmp_part(input_file, cropped_output_file, x=0, y=0, width=48, height=48)

    # Open the cropped BMP file and get its pixel data
    cropped_img = Image.open(cropped_output_file)
    image_data = list(cropped_img.getdata())

    # Export the cropped image data to a C file
    export_to_c_format(image_data, width=48, height=48, output_file=c_output_file)
