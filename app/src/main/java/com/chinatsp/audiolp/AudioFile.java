package com.chinatsp.audiolp;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class AudioFile {

    private File mAudioFile;
    private FileOutputStream mFileOutStream;
    private String mFilePath;
    private boolean mIsRecording;

    // Constructor to create a new file instance
    public AudioFile(String filePath, boolean isRecord) throws IOException {
        mFilePath = filePath;
        mIsRecording = isRecord;
        if(mIsRecording) {
            init();
        }
    }

    private void init() {
        mAudioFile = new File(mFilePath);

        try {
            // Create the file if it does not exist
            if (!mAudioFile.exists()) {
                mAudioFile.createNewFile();
            }

            // Initialize the file output stream
            mFileOutStream = new FileOutputStream(mAudioFile);
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }

    }

    // Method to write data to the file
    public void writeData(byte[] data, int dataLen) throws IOException {
        if (!mIsRecording) {
            return;
        }
        if (mFileOutStream != null) {
            mFileOutStream.write(data);
        } else {
            throw new IOException("FileOutputStream is not initialized");
        }
    }

    // Method to close the file output stream
    public void close() throws IOException {
        if (!mIsRecording) {
            return;
        }
        if (mFileOutStream != null) {
            mFileOutStream.close();
        } else {
            throw new IOException("FileOutputStream is not initialized");
        }
    }

    // Getter for the audio file
    public File getAudioFile() {
        return mAudioFile;
    }
}
