import wave, struct, math, sys

def main():
    out = sys.argv[1] if len(sys.argv) > 1 else 'input.wav'
    sr = 44100
    sec = 2
    n = sr * sec
    w = wave.open(out, 'wb')
    w.setnchannels(2)
    w.setsampwidth(2)
    w.setframerate(sr)
    for i in range(n):
        t = i / sr
        x = int(0.1 * 32767 * math.sin(2*math.pi*220*t))
        w.writeframes(struct.pack('<hh', x, x))
    w.close()
    print(f'generated {out}')

if __name__ == '__main__':
    main()

