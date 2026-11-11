package com.huntares.external;

import android.app.Activity;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.content.Intent;
import android.widget.*;
import android.view.Gravity;
import android.view.ViewGroup;
import android.graphics.BitmapFactory;
import java.io.*;
import java.nio.file.Files;

public final class MainActivity extends Activity {
    private TextView status;
    private final Handler handler = new Handler(Looper.getMainLooper());
    private static final String ENGINE = "/data/local/tmp/huntares-engine";
    private int dp(int n) { return Math.round(getResources().getDisplayMetrics().density * n); }
    private GradientDrawable bg(int color, int radius) {
        GradientDrawable d = new GradientDrawable(); d.setColor(color); d.setCornerRadius(dp(radius)); return d;
    }
    private TextView text(String s, int size, int color, boolean bold) {
        TextView v = new TextView(this); v.setText(s); v.setTextSize(size); v.setTextColor(color);
        if(bold) v.setTypeface(Typeface.DEFAULT,Typeface.BOLD); return v;
    }
    private void runAsync(String action, Action task) {
        status.setText(action + "...");
        new Thread(() -> {
            String message;
            try { message=task.execute(); } catch(Exception ex) { message="Ошибка: "+ex.getMessage(); }
            final String result=message; handler.post(() -> status.setText(result));
        }).start();
    }
    private interface Action { String execute() throws Exception; }
    private String shell(String command) throws Exception {
        Process p=new ProcessBuilder("su","-c",command).redirectErrorStream(true).start();
        ByteArrayOutputStream bytes=new ByteArrayOutputStream();
        try(InputStream in=p.getInputStream()) { byte[] b=new byte[2048];int n;
            while((n=in.read(b))!=-1 && bytes.size()<65536) bytes.write(b,0,n); }
        int code=p.waitFor();
        String output=bytes.toString("UTF-8").trim();
        if(code!=0) throw new IllegalStateException("root exit="+code+": "+output);
        return output;
    }
    private String prepareEngine() throws Exception {
        File local=new File(getFilesDir(),"huntares-engine");
        try(InputStream from=getAssets().open("bin/huntares-engine");
            FileOutputStream to=new FileOutputStream(local)) {
            byte[] b=new byte[32768];int n;
            while((n=from.read(b))!=-1) to.write(b,0,n);
        }
        String path=local.getAbsolutePath();
        shell("cp '"+path+"' '"+ENGINE+"' && chmod 700 '"+ENGINE+"'");
        return ENGINE;
    }
    private void launchGame() {
        Intent launch=getPackageManager().getLaunchIntentForPackage("com.axlebolt.standoff2");
        if(launch!=null) startActivity(launch);
        else status.setText("Standoff 2 не найден");
    }
    @Override public void onCreate(Bundle state) {
        super.onCreate(state); getWindow().setStatusBarColor(Color.BLACK);getWindow().setNavigationBarColor(Color.BLACK);
        LinearLayout root=new LinearLayout(this);root.setOrientation(LinearLayout.VERTICAL);
        root.setPadding(dp(24),dp(35),dp(24),dp(24));root.setBackgroundColor(Color.rgb(12,12,14));
        root.setGravity(Gravity.CENTER_HORIZONTAL);setContentView(root);
        ImageView art=new ImageView(this);art.setImageResource(com.huntares.external.R.drawable.ic_huntares);
        art.setScaleType(ImageView.ScaleType.CENTER_CROP);
        LinearLayout.LayoutParams ap=new LinearLayout.LayoutParams(dp(104),dp(104));ap.bottomMargin=dp(24);
        art.setBackground(bg(Color.rgb(34,34,37),50));art.setClipToOutline(true);root.addView(art,ap);
        TextView title=text("HUNTARES",32,Color.WHITE,true);title.setGravity(Gravity.CENTER);root.addView(title);
        TextView subtitle=text("E X T E R N A L     •     2 D   B O X",11,Color.LTGRAY,false);
        subtitle.setGravity(Gravity.CENTER);LinearLayout.LayoutParams sp=new LinearLayout.LayoutParams(-1,dp(48));
        root.addView(subtitle,sp);
        TextView description=text("ESP 2D Box  •  Native ARM64  •  Root",14,Color.LTGRAY,false);
        description.setGravity(Gravity.CENTER);LinearLayout.LayoutParams desc=new LinearLayout.LayoutParams(-1,dp(68));
        root.addView(description,desc);
        Button start=button("ЗАПУСТИТЬ HUNTARES",root,true);
        start.setOnClickListener(v->runAsync("Запуск",()->{
            prepareEngine();
            shell("( nohup '"+ENGINE+"' > /data/local/tmp/huntares.log 2>&1 < /dev/null & )");
            handler.postDelayed(this::launchGame,600);
            return "Модуль запущен. Проверяй статус ESP в логе.";
        }));
        Button game=button("ОТКРЫТЬ ИГРУ",root,false);
        game.setOnClickListener(v->launchGame());
        Button logs=button("ПОСМОТРЕТЬ ЛОГ",root,false);
        logs.setOnClickListener(v->runAsync("Чтение лога",()->shell("tail -n 25 /data/local/tmp/huntares.log 2>/dev/null || true")));
        Button stop=button("ОСТАНОВИТЬ МОДУЛЬ",root,false);
        stop.setOnClickListener(v->runAsync("Остановка",()->{
            shell("pkill -f '^"+ENGINE+"$' || true");return "Модуль остановлен";
        }));
        status=text("Для запуска нужен root (KernelSU / Magisk).",12,Color.LTGRAY,false);
        status.setGravity(Gravity.CENTER);status.setPadding(dp(8),dp(18),dp(8),dp(5));root.addView(status);
    }
    private Button button(String label, LinearLayout root, boolean primary) {
        Button b=new Button(this);b.setText(label);b.setAllCaps(false);b.setTextSize(14);
        b.setTextColor(primary?Color.BLACK:Color.WHITE);
        b.setBackground(bg(primary?Color.WHITE:Color.rgb(36,36,39),12));
        LinearLayout.LayoutParams p=new LinearLayout.LayoutParams(-1,dp(52));p.bottomMargin=dp(12);
        root.addView(b,p);return b;
    }
}
