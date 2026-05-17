#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <geopack.h>

namespace {

std::string fmt(double v) {
    if (!std::isfinite(v)) {
        return "null";
    }
    std::ostringstream oss;
    oss << std::setprecision(17) << v;
    return oss.str();
}

std::string fmtf(float v) {
    std::ostringstream oss;
    oss << std::setprecision(9) << v;
    return oss.str();
}

void writeFile(const std::string &path, const std::string &data) {
    std::ofstream f(path);
    f << data;
}

void saveDipoleTilt(const std::string &outdir) {
    struct Case {
        int date;
        float ut;
        double vx;
        double vy;
        double vz;
    };
    const std::vector<Case> cases = {
        {20120101, 0.0f, -400.0, 0.0, 0.0},
        {20120101, 12.0f, -359.0, 11.0, -17.4},
        {20160320, 6.5f, -420.0, 5.0, -10.0},
    };

    std::ostringstream out;
    out << "{\n  \"api\":\"GetDipoleTiltUT\",\n  \"cases\":[\n";
    for (size_t i = 0; i < cases.size(); i++) {
        const auto &c = cases[i];
        const double tilt = GetDipoleTiltUT(c.date, c.ut, c.vx, c.vy, c.vz);
        out << "    {\"date\":" << c.date << ",\"ut\":" << fmtf(c.ut)
            << ",\"vx\":" << fmt(c.vx) << ",\"vy\":" << fmt(c.vy)
            << ",\"vz\":" << fmt(c.vz) << ",\"tilt\":" << fmt(tilt) << "}";
        if (i + 1 < cases.size()) {
            out << ",";
        }
        out << "\n";
    }
    out << "  ]\n}\n";
    writeFile(outdir + "/getdipoletiltut.json", out.str());
}

void saveConvCoords(const std::string &outdir) {
    const int n = 3;
    double xin[n] = {1.0, -2.0, 3.5};
    double yin[n] = {0.0, 1.0, -0.2};
    double zin[n] = {2.0, -1.0, 0.3};
    int date[n] = {20120101, 20120101, 20160320};
    float ut[n] = {12.0f, 12.0f, 6.5f};
    double vx[n] = {-359.0, -359.0, -420.0};
    double vy[n] = {11.0, 11.0, 5.0};
    double vz[n] = {-17.4, -17.4, -10.0};

    struct Pair {
        const char *in;
        const char *out;
    };
    const std::vector<Pair> pairs = {
        {"GSM", "GSE"},
        {"GSE", "SM"},
        {"GEO", "MAG"},
    };

    std::ostringstream out;
    out << "{\n  \"api\":\"ConvCoords\",\n  \"cases\":[\n";
    for (size_t p = 0; p < pairs.size(); p++) {
        double xout[n] = {0.0, 0.0, 0.0};
        double yout[n] = {0.0, 0.0, 0.0};
        double zout[n] = {0.0, 0.0, 0.0};
        ConvCoords(xin, yin, zin, n, vx, vy, vz, date, ut, xout, yout, zout, pairs[p].in, pairs[p].out);

        out << "    {\"coord_in\":\"" << pairs[p].in << "\",\"coord_out\":\"" << pairs[p].out << "\",";
        out << "\"n\":" << n << ",";
        out << "\"xin\":[" << fmt(xin[0]) << "," << fmt(xin[1]) << "," << fmt(xin[2]) << "],";
        out << "\"yin\":[" << fmt(yin[0]) << "," << fmt(yin[1]) << "," << fmt(yin[2]) << "],";
        out << "\"zin\":[" << fmt(zin[0]) << "," << fmt(zin[1]) << "," << fmt(zin[2]) << "],";
        out << "\"date\":[" << date[0] << "," << date[1] << "," << date[2] << "],";
        out << "\"ut\":[" << fmtf(ut[0]) << "," << fmtf(ut[1]) << "," << fmtf(ut[2]) << "],";
        out << "\"vx\":[" << fmt(vx[0]) << "," << fmt(vx[1]) << "," << fmt(vx[2]) << "],";
        out << "\"vy\":[" << fmt(vy[0]) << "," << fmt(vy[1]) << "," << fmt(vy[2]) << "],";
        out << "\"vz\":[" << fmt(vz[0]) << "," << fmt(vz[1]) << "," << fmt(vz[2]) << "],";
        out << "\"xout\":[" << fmt(xout[0]) << "," << fmt(xout[1]) << "," << fmt(xout[2]) << "],";
        out << "\"yout\":[" << fmt(yout[0]) << "," << fmt(yout[1]) << "," << fmt(yout[2]) << "],";
        out << "\"zout\":[" << fmt(zout[0]) << "," << fmt(zout[1]) << "," << fmt(zout[2]) << "]}";
        if (p + 1 < pairs.size()) {
            out << ",";
        }
        out << "\n";
    }
    out << "  ]\n}\n";
    writeFile(outdir + "/convcoords.json", out.str());
}

void saveModelField(const std::string &outdir) {
    const int n = 2;
    const char *models[] = {"T89", "T96", "T01", "TS05"};
    int date[n] = {20120101, 20160320};
    float ut[n] = {12.0f, 6.5f};
    double xin[n] = {0.0, 2.0};
    double yin[n] = {10.0, -3.0};
    double zin[n] = {0.0, 1.2};
    int iopt[n] = {2, 2};
    double vx[n] = {-359.0, -420.0};
    double vy[n] = {11.0, 5.0};
    double vz[n] = {-17.4, -10.0};

    std::ostringstream out;
    out << "{\n  \"api\":\"ModelField\",\n  \"cases\":[\n";

    for (int m = 0; m < 4; m++) {
        double row0[10] = {2.5, -15.0, 1.0, -2.0, 0.3, -0.2, 0.1, 0.0, 0.0, 0.0};
        double row1[10] = {3.0, -20.0, 0.8, -1.2, 0.2, -0.1, 0.2, 0.1, 0.0, -0.1};
        double *parmod[n] = {row0, row1};
        double bx[n] = {0.0, 0.0};
        double by[n] = {0.0, 0.0};
        double bz[n] = {0.0, 0.0};
        ModelField(n, xin, yin, zin, date, ut, false, models[m], iopt, parmod, vx, vy, vz, "GSM", "GSM", false, bx, by, bz);

        out << "    {\"model\":\"" << models[m] << "\",\"same_time\":0,\"n\":" << n << ",";
        out << "\"xin\":[" << fmt(xin[0]) << "," << fmt(xin[1]) << "],";
        out << "\"yin\":[" << fmt(yin[0]) << "," << fmt(yin[1]) << "],";
        out << "\"zin\":[" << fmt(zin[0]) << "," << fmt(zin[1]) << "],";
        out << "\"date\":[" << date[0] << "," << date[1] << "],";
        out << "\"ut\":[" << fmtf(ut[0]) << "," << fmtf(ut[1]) << "],";
        out << "\"vx\":[" << fmt(vx[0]) << "," << fmt(vx[1]) << "],";
        out << "\"vy\":[" << fmt(vy[0]) << "," << fmt(vy[1]) << "],";
        out << "\"vz\":[" << fmt(vz[0]) << "," << fmt(vz[1]) << "],";
        out << "\"iopt\":[" << iopt[0] << "," << iopt[1] << "],";
        out << "\"parmod\":[[";
        for (int j = 0; j < 10; j++) {
            if (j) out << ",";
            out << fmt(row0[j]);
        }
        out << "],[";
        for (int j = 0; j < 10; j++) {
            if (j) out << ",";
            out << fmt(row1[j]);
        }
        out << "]],";
        out << "\"bx\":[" << fmt(bx[0]) << "," << fmt(bx[1]) << "],";
        out << "\"by\":[" << fmt(by[0]) << "," << fmt(by[1]) << "],";
        out << "\"bz\":[" << fmt(bz[0]) << "," << fmt(bz[1]) << "]}";
        out << ",\n";
    }

    {
        const int n1 = 2;
        int date1[1] = {20120101};
        float ut1[1] = {12.0f};
        double xin1[n1] = {0.0, 1.5};
        double yin1[n1] = {10.0, -4.0};
        double zin1[n1] = {0.0, 2.0};
        int iopt1[1] = {2};
        double vx1[1] = {-359.0};
        double vy1[1] = {11.0};
        double vz1[1] = {-17.4};
        double row[10] = {2.5, -15.0, 1.0, -2.0, 0.3, -0.2, 0.1, 0.0, 0.0, 0.0};
        double *parmod1[1] = {row};
        double bx[n1] = {0.0, 0.0};
        double by[n1] = {0.0, 0.0};
        double bz[n1] = {0.0, 0.0};
        ModelField(n1, xin1, yin1, zin1, date1, ut1, true, "T96", iopt1, parmod1, vx1, vy1, vz1, "GSM", "GSM", false, bx, by, bz);

        out << "    {\"model\":\"T96\",\"same_time\":1,\"n\":" << n1 << ",";
        out << "\"xin\":[" << fmt(xin1[0]) << "," << fmt(xin1[1]) << "],";
        out << "\"yin\":[" << fmt(yin1[0]) << "," << fmt(yin1[1]) << "],";
        out << "\"zin\":[" << fmt(zin1[0]) << "," << fmt(zin1[1]) << "],";
        out << "\"date\":[" << date1[0] << "],";
        out << "\"ut\":[" << fmtf(ut1[0]) << "],";
        out << "\"vx\":[" << fmt(vx1[0]) << "],";
        out << "\"vy\":[" << fmt(vy1[0]) << "],";
        out << "\"vz\":[" << fmt(vz1[0]) << "],";
        out << "\"iopt\":[" << iopt1[0] << "],";
        out << "\"parmod\":[[";
        for (int j = 0; j < 10; j++) {
            if (j) out << ",";
            out << fmt(row[j]);
        }
        out << "]],";
        out << "\"bx\":[" << fmt(bx[0]) << "," << fmt(bx[1]) << "],";
        out << "\"by\":[" << fmt(by[0]) << "," << fmt(by[1]) << "],";
        out << "\"bz\":[" << fmt(bz[0]) << "," << fmt(bz[1]) << "]}\n";
    }

    out << "  ]\n}\n";
    writeFile(outdir + "/modelfield.json", out.str());
}

void saveTraceField(const std::string &outdir) {
    const char *models[] = {"T89", "T96", "T01", "TS05"};
    std::ostringstream out;
    out << "{\n  \"api\":\"TraceField\",\n  \"cases\":[\n";

    for (int m = 0; m < 4; m++) {
        int n = 1;
        int date[] = {20120101};
        float ut[] = {12.0f};
        double xin[] = {0.0};
        double yin[] = {8.0};
        double zin[] = {0.1};
        int iopt[] = {2};
        double vx[] = {-359.0};
        double vy[] = {11.0};
        double vz[] = {-17.4};
        double row[10] = {2.5, -15.0, 1.0, -2.0, 0.3, -0.2, 0.1, 0.05, 0.0, -0.03};
        double *parmod[] = {row};

        int maxLen = 256;
        int nstep = 0;
        double *xgsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *ygsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *zgsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bxgsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bygsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bzgsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *xgse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *ygse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *zgse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bxgse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bygse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bzgse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *xsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *ysm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *zsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bxsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bysm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bzsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *s = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *r = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *rnorm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *fp = static_cast<double *>(malloc(maxLen * sizeof(double)));
        int nalpha = 2;
        double alpha[] = {0.0, 90.0};
        double halpha[512] = {0.0};

        TraceField(n, xin, yin, zin, date, ut, models[m], iopt, parmod, vx, vy, vz,
                   100.0, maxLen, 1.0, false, 0, "GSM", &nstep, &xgsm, &ygsm, &zgsm,
                   &bxgsm, &bygsm, &bzgsm, &xgse, &ygse, &zgse, &bxgse, &bygse, &bzgse,
                   &xsm, &ysm, &zsm, &bxsm, &bysm, &bzsm, &s, &r, &rnorm, &fp, nalpha, alpha, halpha);

        out << "    {\"model\":\"" << models[m] << "\",\"nstep\":" << nstep << ",";
        out << "\"date\":" << date[0] << ",\"ut\":" << fmtf(ut[0]) << ",\"x0\":" << fmt(xin[0])
            << ",\"y0\":" << fmt(yin[0]) << ",\"z0\":" << fmt(zin[0]) << ",";
        out << "\"iopt\":" << iopt[0] << ",\"parmod\":[";
        for (int j = 0; j < 10; j++) {
            if (j) out << ",";
            out << fmt(row[j]);
        }
        out << "],";

        auto writeArr = [&](const char *name, double *arr) {
            out << "\"" << name << "\":[";
            for (int i = 0; i < nstep; i++) {
                if (i) out << ",";
                out << fmt(arr[i]);
            }
            out << "]";
        };

        writeArr("xgsm", xgsm); out << ",";
        writeArr("ygsm", ygsm); out << ",";
        writeArr("zgsm", zgsm); out << ",";
        writeArr("bxgsm", bxgsm); out << ",";
        writeArr("bygsm", bygsm); out << ",";
        writeArr("bzgsm", bzgsm); out << ",";
        writeArr("xgse", xgse); out << ",";
        writeArr("ygse", ygse); out << ",";
        writeArr("zgse", zgse); out << ",";
        writeArr("xsm", xsm); out << ",";
        writeArr("ysm", ysm); out << ",";
        writeArr("zsm", zsm); out << ",";
        writeArr("s", s); out << ",";
        writeArr("r", r); out << ",";
        writeArr("rnorm", rnorm); out << ",";
        writeArr("fp", fp);
        out << "}";
        if (m < 3) {
            out << ",";
        }
        out << "\n";

        free(xgsm); free(ygsm); free(zgsm);
        free(bxgsm); free(bygsm); free(bzgsm);
        free(xgse); free(ygse); free(zgse);
        free(bxgse); free(bygse); free(bzgse);
        free(xsm); free(ysm); free(zsm);
        free(bxsm); free(bysm); free(bzsm);
        free(s); free(r); free(rnorm); free(fp);
    }
    out << "  ]\n}\n";
    writeFile(outdir + "/tracefield.json", out.str());
}

}  // namespace

int main() {
    std::filesystem::create_directories("../data");
    const std::string outdir = "../data";
    saveDipoleTilt(outdir);
    saveConvCoords(outdir);
    saveModelField(outdir);
    saveTraceField(outdir);
    return 0;
}
