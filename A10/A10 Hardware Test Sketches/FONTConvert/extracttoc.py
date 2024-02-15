# 20240216 Now need to roll though all didgts and create array with correct headings etc

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
                if (pixel_0 == 0):
                    pixel_0 = 1
                else:
                    pixel_0 = 0
                print("Pixel_0 = " + str(pixel_0))
                
                pixel_1 = image_data[y * width + x + 1]
                if (pixel_1 == 0):
                    pixel_1 = 1
                else:
                    pixel_1 = 0
                print("Pixel_1 = " + str(pixel_1))
                
                pixel_2 = image_data[y * width + x + 2]
                if (pixel_2 == 0):
                    pixel_2 = 1
                else:
                    pixel_2 = 0
                print("Pixel_2 = " + str(pixel_2))
                
                pixel_3 = image_data[y * width + x + 3]
                if (pixel_3 == 0):
                    pixel_3 = 1
                else:
                    pixel_3 = 0
                print("Pixel_3 = " + str(pixel_3))
                
                pixel_4 = image_data[y * width + x + 4]
                if (pixel_4 == 0):
                    pixel_4 = 1
                else:
                    pixel_4 = 0
                print("Pixel_4 = " + str(pixel_4))
                
                pixel_5 = image_data[y * width + x + 5]
                if (pixel_5 == 0):
                    pixel_5 = 1
                else:
                    pixel_5 = 0                
                print("Pixel_5 = " + str(pixel_5))

                    
                pixel_6 = image_data[y * width + x + 6]
                if (pixel_6 == 0):
                    pixel_6 = 1
                else:
                    pixel_6 = 0
                
                print("Pixel_6 = " + str(pixel_6))
                
                pixel_7 = image_data[y * width + x + 7]
                if (pixel_7 == 0):
                    pixel_7 = 1
                else:
                    pixel_7 = 0               
                print("Pixel_7 = " + str(pixel_7))

                #pixel = pixel_7 * 128 + pixel_6 * 64 + pixel_5 * 32 + pixel_4 * 16 + pixel_3 * 8 + pixel_2 * 4 + pixel_1 * 2 + pixel_0
                pixel = pixel_0 * 128 + pixel_1 * 64 + pixel_2 * 32 + pixel_3 * 16 + pixel_4 * 8 + pixel_5 * 4 + pixel_6 * 2 + pixel_7
                
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
    rotated_img = cropped_img.transpose(method=Image.ROTATE_90)
    flipped_img = rotated_img.transpose(Image.FLIP_TOP_BOTTOM)
    image_data = list(flipped_img.getdata())

    # Export the cropped image data to a C file
    export_to_c_format(image_data, width=48, height=48, output_file=c_output_file)
