#ifndef DECRYPTOR_H
#define DECRYPTOR_H

#include "FileHeaderBlock.h"



class StarsRandom {
public:
    // We'll use 'long' for our seeds to avoid signed-integer problems
    long seedA;
    long seedB;

    int rounds;
    StarsRandom() {};

    void setRandom(int prime1, int prime2, int initRounds);

    // Get the next random number with this seeded generator
    long nextRandom();
};

static const int primes[] = {
    3, 5, 7, 11, 13, 17, 19, 23,
    29, 31, 37, 41, 43, 47, 53, 59,
    61, 67, 71, 73, 79, 83, 89, 97,
    101, 103, 107, 109, 113, 127, 131, 137,
    139, 149, 151, 157, 163, 167, 173, 179,
    181, 191, 193, 197, 199, 211, 223, 227,
    229, 233, 239, 241, 251, 257, 263, 279,
    271, 277, 281, 283, 293, 307, 311, 313
};


class Decryptor
{
public:
    Decryptor();
    static const int BLOCK_HEADER_SIZE = 2;  // bytes
    static const int BLOCK_MAX_SIZE = 1024;  // bytes

    /**
     * The first 64 prime numbers, after '2' (so all are odd). These are used
     * as starting seeds to the random number generator.
     *
     * IMPORTANT:  One number here is not prime (279).  I thought it should be
     * replaced with 269, which is prime.  StarsHostEditor 0.3 decompiled source
     * uses 279 and it turns out that an analysis of the stars EXE with a hex editor
     * also shows a primes table with 279.  Fun!
     */


    StarsRandom starsRandom;

    void decodeBytesForStarsString(int, unsigned char *, int *, unsigned char *);

    /**
     * Initialize the decryption system by seeding and initializing a
     * random number generator
     * @param block
     *
     * @throws Exception
     */
    void initDecryption(FileHeaderBlock fileHeaderBlock);

    /**
     * Decrypt the given block.
     *
     * The first call to this will be the File Header Block which will
     * be used to initialize the decryption system
     *
     * @param block
     * @throws Exception
     */
    void decryptBlock(Block block);
    /**
     * Encrypt the given block.
     *
     * The first call to this will be the File Header Block which will
     * be used to initialize the encryption system
     *
     * @param block
     * @throws Exception
     */
    void encryptBlock(Block block);

    /**
     * This will detect and return a block with its type, size, and block of the
     * given block from the given data
     *
     * Details of a header block bitwise: XXXXXXXX YYYYYYZZ
     *   (XXXXXXXX is a first byte, YYYYYYZZ is a second byte)
     *
     * Where:
     *   YYYYYY is a block type.
     *   ZZXXXXXXXX is a block size.
     *
     * @param offset
     * @param fileBytes
     * @return
     * @throws Exception
     */
    Block parseBlock(char* fileBytes, int offset);

    /**
     * Some blocks have more data at the end of the block (like PLANETS).
     * Detect this, parse the data, and return the size of the data.
     *
     * Requires decryption to have been done on the block data
     *
     * @param offset
     * @param fileBytes
     * @param block
     * @return
     */
    int postProcessBlock(char* fileBytes, int offset, Block block);


    /**
     * Read in a Stars! file to decrypt it.  This returns a List of all blocks found
     * within.  Each encrypted block will be decrypted.
     *
     * @param filename
     * @return
     * @throws Exception
     */
    //List<Block> readFile(String filename);

    /**
     * Write a block to an output stream.  Assume already encrypted if needed.
     */
    /*private void writeBlock(OutputStream out, Block block) throws Exception {
        int header = (block.typeId << 10) | block.size;
        out.write(header & 0xFF);
        out.write((header >> 8) & 0xFF);
        out.write(block.getData(), 0, block.size);
        if(block.typeId == BlockType.PLANETS) {
            PlanetsBlock planetsBlock = (PlanetsBlock) block;
            out.write(planetsBlock.planetsData);
        }
    }*/

    /**
     * Write blocks to an output stream.  Blocks must be previously encoded.  This method will encrypt as needed.
     */
    /*public void writeBlocks(OutputStream out, List<Block> blocks, boolean updateFileHashBlock) throws Exception {
        if (updateFileHashBlock){ //Is needed to correctly write to x-files!
            Block fileHashBlock = null;
            int nrOfBytes = 0;
            for (Block block : blocks){
                if (block instanceof FileHashBlock){
                    fileHashBlock = (FileHashBlock)block;
                }
                else if (fileHashBlock != null){
                    nrOfBytes += block.size+2;
                }
            }
            if (fileHashBlock != null){
                System.out.println("Writing "+nrOfBytes+" to FileHashBlock");
                byte[] b = fileHashBlock.getDecryptedData();
                Util.write16(b, 0, nrOfBytes);
            }
        }
        for (Block block : blocks) {
            encryptBlock(block);
            writeBlock(out, block);
        }
    }*/

    /**
     * Write blocks to an file.  Encrypt as needed.
     */
    /*public void writeBlocks(String filename, List<Block> blocks, boolean updateFileHashBlock) throws Exception {
        OutputStream out = new BufferedOutputStream(new FileOutputStream(filename));
        try {
            writeBlocks(out, blocks, updateFileHashBlock);
        } finally {
            out.close();
        }
    }*/
};

#endif // DECRYPTOR_H
