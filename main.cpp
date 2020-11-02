#include <cstdio>
#include <string>
#include <dirent.h>

#define i16 short int
#define u8 unsigned char
#define u32 unsigned int
#define u16 unsigned short int

template<typename T>
struct Vec4 {
    T x, y, z, w;
};
    
struct Voxel {
    u32 color;
    Vec4<i16> coordinates;
};

static u16 SLICE_WIDTH = 256;
static u16 SLICE_HEIGHT = 256;
static u16 SLICE_COUNT = 1;
static u8 THRESHOLD = 0;
static u8 DEPTH_SIZE = 1;
static u8 RESCALE_LUMINOSITY_FROM = 0;
static u16 BIT_AND_MASK = 0x00FF;
static u16 BIT_XOR_MASK = 0x0000;
//static bool THRESHOLD_AUTO = false;
static u32 SLICE_VOXEL_COUNT;
static u32 DATA_VOXEL_COUNT;
static bool TRANSPARENT_VOXELS = false;

int main(int argc, char** argv) {
    
    int i = 2;
    while(i < argc) {
        const std::string name = argv[i];
        if(name.find("slice-width:") == 0) {
           SLICE_WIDTH = std::stoi(name.substr(12));
        } else if(name.find("slice-height:") == 0) {
           SLICE_HEIGHT = std::stoi(name.substr(13));
        } else if(name.find("slice-count:") == 0) {
           SLICE_COUNT = std::stoi(name.substr(12));
        } else if(name.find("threshold:") == 0) {
           THRESHOLD = std::stoi(name.substr(10));
        } else if(name.find("depth-size:") == 0) {
           DEPTH_SIZE = std::stoi(name.substr(11));
        } else if(name.find("rescale-luminosity-from:") == 0) {
            RESCALE_LUMINOSITY_FROM = stoi(name.substr(24));
        } else if(name.find("bit-and-mask:") == 0) {
            //0x000F
            BIT_AND_MASK = stoll(name.substr(13), 0 , 16);
        } else if(name.find("bit-xor-mask:") == 0) {
            BIT_XOR_MASK = stoll(name.substr(13), 0 , 16);
        } else if(name.find("transparent-voxels") == 0) {
            TRANSPARENT_VOXELS = true;
        }
        i++;
    }
    
    
    
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(argv[1])) != NULL) {
        printf("Scanning %s folder...\n", argv[1]);
        SLICE_VOXEL_COUNT = SLICE_WIDTH * SLICE_HEIGHT;
        DATA_VOXEL_COUNT = SLICE_VOXEL_COUNT * SLICE_COUNT;
        u16* const data = new u16[DATA_VOXEL_COUNT];
        
        u16 ns = 0;
        while((ent = readdir(dir)) != NULL) {
            std::string name = ent->d_name;
            if(name != ".." && name != ".") {
                std::size_t pos = name.find_last_of(".");
                if(pos != std::string::npos) {
                    std::string filename = argv[1];
                    filename += "/" + name;
                    FILE* const f = fopen(filename.c_str(), "rb");
                    if(f != NULL) {
                        const u32 sid = std::stoi(name.substr(pos + 1)) - 1;
                        const u32 offset = sid * SLICE_VOXEL_COUNT;
                        printf("Slice id: %u. File (%s) loaded.\n", sid, name.c_str());
                        fread(&(data[offset]), sizeof(u16), SLICE_VOXEL_COUNT, f);
                        fclose(f);
                        ns++;
                    }
                }
            }
        }
        printf("%u files read\n", ns);
        closedir(dir);
        
        const u16 SLICE_HALF_WIDTH = SLICE_WIDTH / 2;
        const u16 SLICE_HALF_HEIGHT = SLICE_HEIGHT / 2;
        const u16 SLICE_HALF_COUNT = SLICE_COUNT / 2;
        
        FILE* const f = fopen("voxels.bin", "wb");
        if(f != NULL) {
            u16 k = 0;
            while(k < (SLICE_COUNT * DEPTH_SIZE)) {
                u16 i = 0;
                while(i < SLICE_WIDTH) {
                    u16 j = 0;
                    while(j < SLICE_HEIGHT) {
                        const u16 value = data[i + j * SLICE_WIDTH + (k / DEPTH_SIZE)  * SLICE_VOXEL_COUNT];
                        u16 l = ((value & BIT_AND_MASK) ^ BIT_XOR_MASK) << 4;

                        if(RESCALE_LUMINOSITY_FROM > 0) {
                            const float f = (255.0f + RESCALE_LUMINOSITY_FROM) / 255.0f;
                            if(l < RESCALE_LUMINOSITY_FROM) {
                                l = 0;
                            } else {
                                l -= RESCALE_LUMINOSITY_FROM;
                            }
                            l = (u8)(l * f);
                        } 
                        
                        if(l > THRESHOLD) {
                            u32 color = 0x000000FF | l << 8 | l << 16 | l << 24;
                            if(TRANSPARENT_VOXELS) {
                                color &= 0xFFFFFF00 | l;
                            }
                            const Voxel voxel = {
                                color,
                                {
                                    (i16)(i - SLICE_HALF_WIDTH),
                                    (i16)(j - SLICE_HALF_HEIGHT),
                                    (i16)(k - (SLICE_HALF_COUNT * DEPTH_SIZE)),
                                    1
                                }
                            };
                            fwrite(&voxel, sizeof(Voxel), 1, f);
                        }
                        j++;
                    }
                    i++;
                }                
                printf("\rWrite %u / %u", k + 1, SLICE_COUNT * DEPTH_SIZE);
                k++;
            }
            fclose(f);
        }
                
        delete [] data;
    }
    return 0;
}
