package com.senvis.finger.senvisdemo;

import android.app.Service;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;

/**
 * Created by naeza on 2017-11-11.
 */

public class SenvisService extends Service{
    static {
        System.loadLibrary("native-lib2");
    }
    private MyThread2 mythread;

    @Override
    public IBinder onBind(Intent arg0) {
        // TODO Auto-generated method stub
        Log.d("ShinJAE", "onBind");
        return null;
    }

    @Override
    public void onCreate() {
        // TODO Auto-generated method stub
        super.onCreate();
        SpiOpen(0,
                1,
                0,
                0,
                0,
                1,
                1,
                5,
                0,
                1,
                0,
                0,
                100,
                10);
        mythread = new MyThread2();
        mythread.start();
        Log.d("ShinJAE", "onCreate");
    }

    @Override
    public void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
        mythread.flag = false;
        mythread = null;
        Log.d("ShinJAE", "onDestroy");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        // TODO Auto-generated method stub
        String m_str = intent.getAction();

        Log.d("ShinJAE", "onStartCommand");

        return super.onStartCommand(intent, flags, startId);
    }

    public class MyThread2 extends Thread{
        boolean flag = true;
        @Override
        public void run() {
            // TODO Auto-generated method stub
            super.run();

            Log.d("ShinJAE", "Thread");
            //while(flag)
            {
                //Log.d("ShinJAE", String.valueOf(test()));
                try {
                    Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.dummy);
                    Bitmap bitmap2 = FPLoop(bitmap);
                    /*
                    try{

                        File file = new File("test.png");
                        FileOutputStream fos = openFileOutput("test.png" , 0);
                        bitmap2.compress(Bitmap.CompressFormat.PNG, 100 , fos);
                        fos.flush();
                        fos.close();

                        Log.d("ShinJAE", "file ok");
                    }catch(Exception e) {  Log.d("ShinJAE", "file error");}

*/
                    Thread.sleep(500);
                    fin();
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                    Log.d("ShinJAE", e.getMessage());
                }/*
                if(flag){
                    Message msg = new Message();
                    mHandler.sendMessage(msg);
                }*/
            }
        }
    }
    /*
    public native String stringFromJNI();
    public native int FPgetTemp2();
*/
    public native String SpiOpen(int r1, int r2,int r3, int r4,int r5, int r6,int r7, int r8,int r9, int r10,int r11, int r12,int r13, int r14);
    public native Bitmap FPLoop(Bitmap bitmap);
    public native int test();
    public native int fin();
}
