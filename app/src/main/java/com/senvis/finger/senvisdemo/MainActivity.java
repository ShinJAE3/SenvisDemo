package com.senvis.finger.senvisdemo;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InterruptedIOException;
import java.util.Timer;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    ImageView ivFP;
    //EditText etID;
    Thread thread;

    EditText edSVF_DLY2X_OSC;
    EditText edSVF_S_GAIN;
    EditText edSVF_ADC_RSEL;
    EditText edSVF_DYL2X;
    EditText edSVF_CLK_SEL;
    EditText edSVF_PERIOD;
    EditText edSVF_ISOL;
    EditText edSVF_SL_TIME;
    EditText edSVF_NAVI;
    EditText edSVF_SCK_UT;
    EditText edSVF_SLV_STU;
    EditText edSVF_MULTIPLYING;
    EditText edSVF_ADC_REF;
    EditText edSVF_THRESHOLD;

    private TextView mTextView;
    private int nFrame = 0;

    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            getFingerPrint();
        }
    };

    void getFingerPrint() {
        Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.dummy);
        ivFP.setImageBitmap(FPLoop(bitmap));
        //int temp = test();
        /*
        int threshold = 0;
        int temp = FPgetTemp2();
        threshold = Integer.parseInt(edSVF_THRESHOLD.getText().toString());

        Log.d("ShinJAE", "temp = " + temp + "threshold = " + threshold + "Frame = " + nFrame);
        if (temp > threshold) {
            nFrame++;
            if (nFrame == 3) {
                Log.d("ShinJAE", "in frame == 3");
                Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.dummy);
                ivFP.setImageBitmap(FPLoop(bitmap));
            }
        } else
            nFrame=0;
            */
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        mTextView = (TextView) findViewById(R.id.sample_text);
        ivFP = (ImageView) findViewById(R.id.imageView);

        edSVF_DLY2X_OSC = (EditText) findViewById(R.id.edSVF_DLY2X_OSC);
        edSVF_S_GAIN = (EditText) findViewById(R.id.edSVF_S_GAIN);
        edSVF_ADC_RSEL = (EditText) findViewById(R.id.edSVF_ADC_RSEL);
        edSVF_DYL2X = (EditText) findViewById(R.id.edSVF_DYL2X);
        edSVF_CLK_SEL = (EditText) findViewById(R.id.edSVF_CLK_SEL);
        edSVF_PERIOD = (EditText) findViewById(R.id.edSVF_PERIOD);
        edSVF_ISOL = (EditText) findViewById(R.id.edSVF_ISOL);
        edSVF_SL_TIME = (EditText) findViewById(R.id.edSVF_SL_TIME);
        edSVF_NAVI = (EditText) findViewById(R.id.edSVF_NAVI);
        edSVF_SCK_UT = (EditText) findViewById(R.id.edSVF_SCK_UT);
        edSVF_SLV_STU = (EditText) findViewById(R.id.edSVF_SLV_STU);
        edSVF_MULTIPLYING = (EditText) findViewById(R.id.edSVF_MULTIPLYING);
        edSVF_ADC_REF = (EditText) findViewById(R.id.edSVF_ADC_REF);
        edSVF_THRESHOLD = (EditText) findViewById(R.id.edSVF_THRESHOLD);

        thread = new Thread() {
            @Override
            public void run() {
                while(true) {
                    handler.sendMessage(handler.obtainMessage());

                    try {
                        Thread.sleep(1000);
                    } catch (Exception e) {
                    }
                }
            }
        };

/*
        Button btnJni = (Button) findViewById(R.id.button);
        btnJni.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mTextView.setText(stringFromJNI());
            }
        });

*/
        Button btnSpi = (Button) findViewById(R.id.button2);
        btnSpi.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try {
                    thread.wait();
                }catch(Exception e){}
                mTextView.setText(SpiOpen(Integer.parseInt(edSVF_DLY2X_OSC.getText().toString()),
                        Integer.parseInt(edSVF_S_GAIN.getText().toString()),
                        Integer.parseInt(edSVF_ADC_RSEL.getText().toString()),
                        Integer.parseInt(edSVF_DYL2X.getText().toString()),
                        Integer.parseInt(edSVF_CLK_SEL.getText().toString()),
                        Integer.parseInt(edSVF_PERIOD.getText().toString()),
                        Integer.parseInt(edSVF_ISOL.getText().toString()),
                        Integer.parseInt(edSVF_SL_TIME.getText().toString()),
                        Integer.parseInt(edSVF_NAVI.getText().toString()),
                        Integer.parseInt(edSVF_SCK_UT.getText().toString()),
                        Integer.parseInt(edSVF_SLV_STU.getText().toString()),
                        Integer.parseInt(edSVF_MULTIPLYING.getText().toString()),
                        Integer.parseInt(edSVF_ADC_REF.getText().toString()),
                        Integer.parseInt(edSVF_THRESHOLD.getText().toString())
                        ));
            }
        });
        //mTextView.setText("SVF10P\n" + SpiOpen());

        //etID = (EditText) findViewById(R.id.editText);

                Button btnFPl = (Button) findViewById(R.id.button3);
                btnFPl.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        try {
                            thread.start();
                        } catch (Exception e) {
                        }

                    /*
                while (true) {
                    Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.dummy);
                    ivFP.setImageBitmap(FPLoop(bitmap));
                    File file = new File("/sdcard/SenvisFP.png");
                    final FileOutputStream filestream = new FileOutputStream(file);
                    bitmap.compress(Bitmap.CompressFormat.PNG, 0, filestream);
                    //mTextView.setText(FPLoop());
                }
                     */
            }
        });

/*
        Button btnPBE = (Button) findViewById(R.id.button4);
        btnFPl.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mTextView.setText(PBTest(Integer.parseInt(etID.getText().toString())));
            }
        });
        */
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native String SpiOpen(int r1, int r2,int r3, int r4,int r5, int r6,int r7, int r8,int r9, int r10,int r11, int r12,int r13, int r14);
    public native Bitmap FPLoop(Bitmap bitmap);
    public native int FPgetTemp2();
    public native int test();
//    public native String PBTest(int id);
}
