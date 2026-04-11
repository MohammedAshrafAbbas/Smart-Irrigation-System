import firebase_admin
from firebase_admin import credentials, db
import pandas as pd
import matplotlib.pyplot as plt

# 1. الاتصال بـ Firebase
try:
    if not firebase_admin._apps:
        cred = credentials.Certificate("key.json")
        firebase_admin.initialize_app(cred, {
            'databaseURL': 'https:.................'
        })
    print("✅ Connection Successful")
except Exception as e:
    print(f"❌ Connection Error: {e}")

def analyze_irrigation():
    ref = db.reference('/sensor_logs')
    data = ref.get()
    
    if not data:
        print("⚠ لا توجد بيانات في قاعدة البيانات حالياً.")
        return

    # تحويل البيانات لجدول ومعالجتها
    df = pd.DataFrame.from_dict(data, orient='index')
    df['datetime'] = pd.to_datetime(df['timestamp'], unit='ms')
    df['hour'] = df['datetime'].dt.hour

    # --- حساب "درجة استجابة الري" (AI Index) ---
    # المعادلة تعطي قيمة عالية إذا كان (التربة جافة + الحرارة منخفضة + الضوء منخفض)
    # نستخدم قيم تقريبية للمعيرة (Normalization)
    df['response_index'] = (
        (df['soil'] / 4095 * 50) +       # وزن رطوبة التربة (50%)
        ((50 - df['temp']) / 50 * 30) +  # وزن الحرارة المنخفضة (30%)
        (df['light'] / 4095 * 20)        # وزن الظلام (20%)
    )

    # إعداد شكل الرسم البياني (4 رسوم في صفحة واحدة)
    fig, axs = plt.subplots(4, 1, figsize=(12, 12), sharex=True)
    plt.subplots_adjust(hspace=0.4)

    # 1. رسم رطوبة التربة
    axs[0].plot(df['datetime'], df['soil'], color='brown', linewidth=2)
    axs[0].set_title('Soil Moisture Level (Higher = Drier)')
    axs[0].grid(True, alpha=0.3)

    # 2. رسم الحرارة والرطوبة الجوية
    axs[1].plot(df['datetime'], df['temp'], color='red', label='Temp (°C)')
    axs[1].plot(df['datetime'], df['hum'], color='blue', label='Humidity (%)')
    axs[1].set_title('Environment: Temperature & Air Humidity')
    axs[1].legend(loc='upper right')
    axs[1].grid(True, alpha=0.3)

    # 3. رسم شدة الضوء
    axs[2].plot(df['datetime'], df['light'], color='orange')
    axs[2].set_title('Light Intensity (Higher = Darker)')
    axs[2].grid(True, alpha=0.3)

    # 4. رسم درجة استجابة الري (المؤشر الذكي)
    axs[3].fill_between(df['datetime'], df['response_index'], color='green', alpha=0.4)
    axs[3].plot(df['datetime'], df['response_index'], color='darkgreen')
    axs[3].set_title('Irrigation Response Index (Optimal Timing Scale 0-100)')
    axs[3].set_ylim(0, 100)
    axs[3].grid(True, alpha=0.3)

    plt.xlabel('Time of Day')
    plt.show()

    # --- تحديد أنسب ساعة للري بناءً على أعلى استجابة ---
    hourly_response = df.groupby('hour')['response_index'].mean()
    best_hour = hourly_response.idxmax()
    
    print("\n" + "="*40)
    print(f"🤖 AI Analysis Result:")
    print(f"أفضل ساعة للري بناءً على الظروف هي الساعة: {best_hour}:00")
    print(f"في هذا الوقت تكون التربة في حاجة للماء مع أقل نسبة تبخر ممكنة.")
    print("="*40)

if __name__ == "__main__":
    analyze_irrigation()





