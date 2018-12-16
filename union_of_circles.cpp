const double EPS = 1e-9;
const double PI  = 3.141592653589793238462;
const double TAU = 2.00000000 * PI;
const double INF = 1e99;
 
int sig(double x) {return x < -EPS ? -1 : x > EPS ? 1 : 0;}
template<class T> T pow2(T x) {return x * x;}
 
 
class Vector {
public:
    double x, y;
    Vector() {}
    Vector(double x, double y): x(x), y(y) {}
 
    Vector operator -() const {return Vector(-x, -y);}
    Vector operator +(const Vector &v) const {return Vector(x+v.x, y+v.y);}
    Vector operator -(const Vector &v) const {return Vector(x-v.x, y-v.y);}
    Vector operator *(const double &s) const {return Vector(x * s, y * s);}
    Vector operator /(const double &s) const {return Vector(x / s, y / s);}
 
    double operator *(const Vector &v) const {return x*v.x + y*v.y;}
    double operator ^(const Vector &v) const {return x*v.y - y*v.x;}
 
    // rotate vector (Right/Left hand)
    Vector  R(double co, double si) {return Vector(x*co-y*si, y*co+x*si);}
    Vector  L(double co, double si) {return Vector(x*co+y*si, y*co-x*si);}
    Vector  R(double th) {return R(cos(th), sin(th));}
    Vector  L(double th) {return L(cos(th), sin(th));}
 
    double len2() {return x*x + y*y;}
    double len()  {return sqrt(len2());}
    double ang()  {return atan2(y, x);}  // angle of vector
    Vector e(double s = 1.0) {return *this / len() * s;}
};
typedef Vector Point;
 
 
class Line {
public:
    Point a, b;
    Line() {}
    Line(Point a, Point b): a(a), b(b) {}
};
 
class Circle {
public:
    Point o;
    double r;
    Circle() {}
    Circle(Point o, double r): o(o), r(r) {}
    int posi(Circle c) {
        double d = (o - c.o).len();
        int in = sig(d - fabs(r - c.r)), ex = sig(d - (r + c.r));
        return in<0 ? -2 : in==0? -1 : ex==0 ? 1 : ex>0? 2 : 0;
    }
 
 
    Line chord(Circle c) {
        Vector v = c.o - o;
        double co = (pow2(r) + v.len2() - pow2(c.r)) / (2 * r * v.len());
        double si = sqrt(fabs(1.000000000 - pow2(co)));
        return Line(v.L(co, si).e(r) + o, v.R(co, si).e(r) + o);
    }
};
 
struct Range {
    double t;
    int evt;
    Point p;
    Range() {}
    Range(double t, int evt, Point p) : t(t), evt(evt), p(p) {}
 
    bool operator <(const Range &s) const {
        return sig(t - s.t) < 0 || (sig(t - s.t) == 0 && evt > s.evt);
    }
};
 
 
const int MAX_N = 1000 + 10;
Circle C[MAX_N];
Range R[MAX_N<<1];
 
bool cmp_r(const Circle &a, const Circle &b) {
    return a.r > b.r;
}
 
double segment_area(double r, double t) {
    return pow2(r) * (t - sin(t)) / 2;
}
 
double union_circle(Circle C[], int &n)
{
    sort(C, C + n, cmp_r);
    int k = 0;
    for (int i = 0; i < n; i++) {
        if (sig(C[i].r) == 0) break;
        int j = 0;
        for (j = 0; j < k; j++)
            if (C[i].posi(C[j]) < 0 || !sig((C[i].o - C[j].o).len()))
                break;
        if (j == k)
            C[k++] = C[i];
    }
    n = k;
 
    double ans = 0;
    for (int i = 0; i < n; ++ i)
    {
        Point mpi = Point(- C[i].r, 0.0) + C[i].o;
        int nc = 0, rcnt = 0;
        R[rcnt++] = Range(-PI,  1, mpi);
        R[rcnt++] = Range( PI, -1, mpi);
        for (int j = 0; j < n; ++ j)
        {
            if (j == i || C[i].posi(C[j])) continue;
 
            Line l = C[i].chord(C[j]);
            double jR = (l.a - C[i].o).ang(), jL = (l.b - C[i].o).ang();
 
            if (sig(jR - jL) > 0) ++ nc;
            R[rcnt++] = Range(jR,  1, l.a);
            R[rcnt++] = Range(jL, -1, l.b);
        }
        sort(R, R + rcnt);
 
        double pj = - PI;
        Point  pp = mpi;
        for(int j = 0; j < rcnt; ++ j)
        {
            nc += R[j].evt;
            if((nc == 2 && R[j].evt > 0) || nc == 0)
                ans += segment_area(C[i].r, R[j].t - pj) + (pp ^ R[j].p) / 2;
            pj = R[j].t; pp = R[j].p;
        }
    }
    return ans;
}

//// usage : union_circle(C,n) 
//// where C is an array of Circle and n is the size of the array ( number of enteries )