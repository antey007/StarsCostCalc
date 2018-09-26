#include <QString>
#include <QDebug>

#include "decryptor.h"

void StarsRandom::setRandom(int prime1, int prime2, int initRounds)
{
    seedA = prime1;
    seedB = prime2;
    rounds = initRounds;

    // Now initialize a few rounds
    for(int i = 0; i < rounds; i++)
        nextRandom();
}

long StarsRandom::nextRandom()
{
    // First, calculate new seeds using some constants
    long seedApartA = (seedA % 53668) * 40014;
    long seedApartB = (seedA / 53668) * 12211;  // integer division OK
    long newSeedA = seedApartA - seedApartB;

    long seedBpartA = (seedB % 52774) * 40692;
    long seedBpartB = (seedB / 52774) * 3791;
    long newSeedB = seedBpartA - seedBpartB;

    // If negative add a whole bunch (there's probably some weird bit math
    // going on here that the disassembler didn't make obvious)
    if(newSeedA < 0)
        newSeedA += 0x7fffffab;

    if(newSeedB < 0)
        newSeedB += 0x7fffff07;

    // Set our new seeds
    seedA = newSeedA;
    seedB = newSeedB;

    // Generate "random" number.  This will fit into an unsigned 32bit integer
    // We use 'long' because...  java...
    long randomNumber = seedA - seedB;
    if(seedA < seedB)
        randomNumber += 0x100000000l;  // 2^32

    // Now return our random number
    return randomNumber;
}

Decryptor::Decryptor()
{
}


char encodesOneByte[] = " aehilnorst";
char encodesB[] = "ABCDEFGHIJKLMNOP";
char encodesC[] = "QRSTUVWXYZ012345";
char encodesD[] = "6789bcdfgjkmpquv";
char encodesE[] = "wxyz+-,!.?:;'*%$";

void Decryptor::decodeBytesForStarsString(int nameLenC, unsigned char * nameC, int * nameLenD, unsigned char *nameD)
{

    char hexChars[256];
    int hexCharsN = 0;
    *nameLenD = 0;
    for (int i = 0; i < nameLenC; i++) {
        hexChars[hexCharsN] = nameC[i]>>4;
        hexChars[hexCharsN+1] = nameC[i] & 0x0F;
        hexCharsN+=2;
    }

    for (int t = 0; t < hexCharsN; t++) {
        char ch1 = hexChars[t];
        if (ch1 == 15){
            if (t < hexCharsN-2) {
                nameD[*nameLenD] = (int)hexChars[t+1] + 16*(int)hexChars[t+2];
                //qDebug() << (int)hexChars[t+1] << (int)hexChars[t+2] << nameD[*nameLenD];
                (*nameLenD)++;
                t+=2;
            }
        }
        else if (ch1 == 14){
            nameD[*nameLenD] = encodesE[hexChars[t+1]];
            (*nameLenD)++;
            t++;
        }
        else if (ch1 == 13){
            nameD[*nameLenD] = encodesD[hexChars[t+1]];
            (*nameLenD)++;
            t++;
        }
        else if (ch1 == 12){
            nameD[*nameLenD] = encodesC[hexChars[t+1]];
            (*nameLenD)++;
            t++;
        }
        else if (ch1 == 11){
            nameD[*nameLenD] = encodesB[hexChars[t+1]];
            (*nameLenD)++;
            t++;
        }
        else{
            nameD[*nameLenD] = encodesOneByte[ch1];
            (*nameLenD)++;
        }
    }
    nameD[*nameLenD] = 0;

}

void Decryptor::initDecryption(FileHeaderBlock fileHeaderBlock)
{
    int salt = fileHeaderBlock.encryptionSalt;

    // Use two prime numbers as random seeds.
    // First one comes from the lower 5 bits of the salt
    int index1 = salt & 0x1F;
    // Second index comes from the next higher 5 bits
    int index2 = (salt >> 5) & 0x1F;

    // Adjust our indexes if the highest bit (bit 11) is set
    // If set, change index1 to use the upper half of our primes table
    if((salt >> 10) == 1)
        index1 += 32;
    // Else index2 uses the upper half of the primes table
    else
        index2 += 32;

    // Determine the number of initialization rounds from 4 other data points
    // 0 or 1 if shareware (I think this is correct, but may not be - so far
    // I have not encountered a shareware flag
    int part1 = fileHeaderBlock.shareware ? 1 : 0;

    // Lower 2 bits of player number, plus 1
    int part2 = (fileHeaderBlock.playerNumber & 0x3) + 1;

    // Lower 2 bits of turn number, plus 1
    int part3 = (fileHeaderBlock.turn & 0x3) + 1;

    // Lower 2 bits of gameId, plus 1
    int part4 = ((int) fileHeaderBlock.gameId & 0x3) + 1;

    // Now put them all together, this could conceivably generate up to 65
    // rounds  (4 * 4 * 4) + 1
    int rounds = (part4 * part3 * part2) + part1;

    // Now initialize our random number generator
    starsRandom.setRandom(primes[index1], primes[index2], rounds);

    // DEBUG
    //		System.out.println(random);
}

/*
void Decryptor::decryptBlock(Decryptor::Block block)
{
    // If it's a header block, it's unencrypted and will be used to
    // initialize the decryption system.  We have to decode it first
    if(block.typeId == BlockType.FILE_HEADER) {
        block.encrypted = false;
        block.decode();

        initDecryption((FileHeaderBlock) block);

        return;
    }

    char* encryptedData = block.getData();

    char* decryptedData = new char[encryptedData.length];

    // Now decrypt, processing 4 bytes at a time
    for(int i = 0; i < encryptedData.length; i+=4) {
        // Swap bytes:  4 3 2 1
        long chunk = (Util.read8(encryptedData[i+3]) << 24)
                | (Util.read8(encryptedData[i+2]) << 16)
                | (Util.read8(encryptedData[i+1]) << 8)
                | Util.read8(encryptedData[i]);

        //			System.out.println("chunk  : " + Integer.toHexString((int)chunk));

        // XOR with a random number
        long decryptedChunk = chunk ^ random.nextRandom();
        //			System.out.println("dechunk: " + Integer.toHexString((int)decryptedChunk));

        // Write out the decrypted data, swapped back
        decryptedData[i] =  (char) (decryptedChunk & 0xFF);
        decryptedData[i+1] =  (char) ((decryptedChunk >> 8)  & 0xFF);
        decryptedData[i+2] =  (char) ((decryptedChunk >> 16)  & 0xFF);
        decryptedData[i+3] =  (char) ((decryptedChunk >> 24)  & 0xFF);
    }

    block.setDecryptedData(decryptedData, block.size);
}

void Decryptor::encryptBlock(Decryptor::Block block)
{        // If it's a header block, it's unencrypted and will be used to
    // initialize the decryption system.  We have to decode it first
    if(block.typeId == BlockType.FILE_HEADER) {
        block.encrypted = false;
        block.decode();

        initDecryption((FileHeaderBlock) block);

        return;
    }

    block.encrypted = true;

    char* decryptedData = block.getDecryptedData();

    char* encryptedData = new char[decryptedData.length];

    // Now encrypt, processing 4 bytes at a time
    for(int i = 0; i < decryptedData.length; i+=4) {
        // Swap bytes:  4 3 2 1
        long chunk = (Util.read8(decryptedData[i+3]) << 24)
                | (Util.read8(decryptedData[i+2]) << 16)
                | (Util.read8(decryptedData[i+1]) << 8)
                | Util.read8(decryptedData[i]);

        //          System.out.println("chunk  : " + Integer.toHexString((int)chunk));

        // XOR with a random number
        long encryptedChunk = chunk ^ random.nextRandom();
        //          System.out.println("dechunk: " + Integer.toHexString((int)decryptedChunk));

        // Write out the decrypted data, swapped back
        encryptedData[i] =  (byte) (encryptedChunk & 0xFF);
        encryptedData[i+1] =  (byte) ((encryptedChunk >> 8)  & 0xFF);
        encryptedData[i+2] =  (byte) ((encryptedChunk >> 16)  & 0xFF);
        encryptedData[i+3] =  (byte) ((encryptedChunk >> 24)  & 0xFF);
    }

    block.setData(encryptedData, block.size);
}

Decryptor::Block Decryptor::parseBlock(char *fileBytes, int offset)
{
    // We have to do a bitwise AND with 0xFF to convert from unsigned byte to int
    int header = Util.read16(fileBytes, offset);

    int typeId = header >> 10;
    int size = header & 0x3FF;

    if(size > BLOCK_MAX_SIZE)
        throw new Exception("Bad block size: " + size + "; typeId: " + typeId);

    // We must have a padded byte array because decryption works on 4
    // bytes at a time
    char* data = new byte[Block.pad(size)];

    // Now copy the block data from the file byte array
    System.arraycopy(fileBytes, offset + 2, data, 0, size);

    // This will create the appropriate Block-type object according to the typeId
    Class<? extends Block> blockClass = BlockType.getBlockClass(typeId);

    Block block = blockClass.newInstance();
    block.setData(data, size);

    return block;
}

int Decryptor::postProcessBlock(char *fileBytes, int offset, Decryptor::Block block)
{
    int size = 0;

    if(block.typeId == BlockType.PLANETS) {
        PlanetsBlock planetsBlock = (PlanetsBlock) block;

        // There are 4 bytes per planet
        size = planetsBlock.planetsSize * 4;
        planetsBlock.planetsDataSize = size;

        planetsBlock.planetsData = Arrays.copyOfRange(fileBytes, offset, offset + size);
    }

    return size;
}

List<Decryptor::Block> Decryptor::readFile(String filename)
{        // Read in the full file to a byte array...  we have the RAM
    File file = new File(filename);
    InputStream fileInputStream = new BufferedInputStream(new FileInputStream(file));
    ByteArrayOutputStream bout = new ByteArrayOutputStream((int) file.length());
    try {
        char* buf = new byte[8192];
        int r;
        while ((r = fileInputStream.read(buf)) > 0) {
            bout.write(buf, 0, r);
        }
    } finally {
        fileInputStream.close();
    }
    byte[] fileBytes = bout.toByteArray();

    // Round 1: Block-parsing
    List<Block> blockList = new ArrayList<Block>();

    // Index where we start to read the next block
    int offset = 0;

    while(offset < fileBytes.length) {
        // Initial parse of our block
        Block block = parseBlock(fileBytes, offset);

        // Do the decryption!
        decryptBlock(block);

        // Decode!
        try {
            block.decode();
        } catch (RuntimeException e) {
            System.out.println("Error (" + e + ") decoding: " + block);
            throw e;
        }

        // Advance our read index
        offset = offset + block.size + BLOCK_HEADER_SIZE;

        // Check to see if we need to grab even more data before the next block
        int dataSize = postProcessBlock(fileBytes, offset, block);

        // Advance the index again
        offset = offset + dataSize;

        // DEBUG
        //System.out.println(block);

        // Store block for later parsing
        blockList.add(block);
    }

    return blockList;

}
*/

