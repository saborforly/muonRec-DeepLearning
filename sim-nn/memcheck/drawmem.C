#if defined(__CINT__) && ! defined(__ACLIC__)
#error "Plase run in compiled mode, such as: root -l -b -q drawmem.C+"
#endif

#include <TSystem.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <TList.h>
#include <TMultiGraph.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

void helper_print(TCanvas* c, const TString& fn, const TString& label, const TString& suffix="") {
    if (suffix=="") {
        c->Print(Form("%s-%s.%s", fn.Data(), label.Data(), "eps"));
        c->Print(Form("%s-%s.%s", fn.Data(), label.Data(), "png"));
    } else {
        c->Print(Form("%s-%s.%s", fn.Data(), label.Data(), suffix.Data()));
    }
}

struct BestUnit {
    TString operator()(double& val, TString unit) {
        if (unit=="s" || unit=="sec" ) {
            if (val<60) { return unit; } // < 1min
            val /= 60;              // s->min
            unit = "min";
        }
        if (unit=="min") {
            if (val<60) { return unit; } // < 1hr
            val /= 60;              // min->hr
            unit = "hr";
        }
        if (unit=="hr") {
            if (val<24) { return unit; } // < 1day
            val /= 24;              // hr->day
            unit = "day";
        }
        return unit;
    }
} best_unit;

// FOR plot, we need to know min/max of the range
static struct Range {
    struct MinMax {
        double min;
        double max;
        int nbins;
        TString unit;

        MinMax() {
            min = DBL_MAX;
            max = DBL_MIN;
            nbins = 100;
        }

        void judge(double val) {
            if (val > max) {
                max = val*1.1;
            }
            if (val < min) {
                min = val*0.9;
            }
        }
    };
    // = time 
    MinMax time_real; // unit: s
    MinMax time_per_evt; // unit: s
    MinMax evt_per_sec; // unit: evts/s

    MinMax cpu_utilization; // 100%

    // = size
    // == size per evt
    MinMax size_per_evt; // unit: MB

    Range() {
    }
} range;

struct OneJob {


    // FLAG: should use this data or not.
    bool use_or_not;  // true -> use, false -> not use
    // job base          (detsim-50220)
    TString job_base;
    // run file          (run-detsim-50220.sh)
    TString fn_run;
    // root file         (detsim-50220.root)
    TString fn_root;
    // log file          (log-detsim-50220.txt)
    TString fn_log;
    // memory usage file (log-detsim-50220.txt.mem.usage)
    TString fn_log_mem;

    // file size in bytes
    Long64_t size_root_edm; // data model
    double szperevt; // size per event
    // time
    double tperevt; // time per event
    double evtpersec; // events per sec

    // data from LOG MEM
    std::vector<double> mem_virt;
    std::vector<double> mem_phys;

    // data from LOG
    TString node;
    int evtmax;
    // output from 'time'
    //   $ time ls
    //   real    0m0.015s
    //   user    0m0.003s
    //   sys     0m0.011s
    struct TimeResult {
        // unit: second
        double real;
        double user;
        double sys;

        double cpu_utilization; // (user+sys)/real (%)

        void cal_utilization() {
            if (0) std::cout << "UTILIZATION: " << user << " " << sys << " " << real << std::endl;
            cpu_utilization = (user+sys)/real*100;
        }

        TimeResult () {
            real = 0.0;
            user = 0.0;
            sys  = 0.0;
            cpu_utilization = 0.0;
        }

        void parse(const std::string& line) {
            std::string label;
            std::string time_str;
            std::stringstream ss;
            ss << line;
            ss >> label >> time_str;

            double t = parse_XmYs(time_str);

            if (label=="real") {
                real = t;
            } else if (label=="user") {
                user = t;
            } else if (label=="sys") {
                sys = t;
            } else {
                std::cerr << "TimeResult::parse: unknown label " << label << std::endl;
            }

        }

        double parse_XmYs(const std::string& s) {
            double t = 0;
            double v = 0;
            TString ss = s;
            TString tok;
            Ssiz_t from = 0;
            while (ss.Tokenize(tok, from, "[ms]")) {
                v = tok.Atof();

                int unit_idx=from-1;
                if (unit_idx<0) {
                    break;
                }
                char unit = ss(unit_idx);

                if (unit == 'm') {
                    t += v*60; // min -> s
                } else if (unit == 's') {
                    t += v;
                } else {
                    std::cerr << "unknown unit: " << unit << std::endl;
                }

                if (0) std::cout << " LTDEBUG: tok: " << tok
                                 << " v: " << v
                                 << " from: " << from
                                 << " ss[from]: " << ss[from]
                                 << std::endl;
            }
            if (0) std::cout << " LTDEBUG orig str: " << ss << " to " << t << " s" << std::endl;
            return t;
        }

    } time_result;

    OneJob() {
        evtmax = -1;
        size_root_edm = 0;
        use_or_not = true;
    }

    void show() {
        std::cout << job_base
                  << ": " << fn_run
                  << " -> " << fn_root
                  << " " << fn_log
                  << " " << fn_log_mem 
                  << " @ " << node
                  << std::endl;
    }

    /*  
     * when a log mem file is given, try to parse it and get job name.
     **/
    void load_log_mem(TString fn) {
        TString dir = gSystem->DirName(fn);
        TString bas = gSystem->BaseName(fn);

        if (0) std::cout << "dir: " << dir << " file: " << bas << std::endl;

        fn_log = bas;
        fn_log.ReplaceAll(".mem.usage", "");

        job_base = fn_log;
        job_base.ReplaceAll("log-", "");
        job_base.ReplaceAll(".txt", "");

        fn_root = Form("%s.root", job_base.Data());
        fn_run = Form("run-%s.sh", job_base.Data());

        fn_log_mem = gSystem->PrependPathName(dir, bas);
        fn_log = gSystem->PrependPathName(dir, fn_log);
        fn_root = gSystem->PrependPathName(dir, fn_root);
        fn_run = gSystem->PrependPathName(dir, fn_run);

        // get size of root file
        int res = gSystem->GetPathInfo(fn_root.Data(), // file path
                                       0,              // id
                                       &size_root_edm, // size
                                       0,              // flag
                                       0               // modtime
                                       );
        if (1) {
            std::cout << "RES: " << res 
                      << " filepath: " << fn_root
                      << " size: " << size_root_edm
                      << std::endl;
        }
        // if RES is not zero, the file may be not exist.
        if (res!=0) {
            use_or_not = false;
        }


        // now parse 
        parse_log_mem();
        parse_log();

        if (!use_or_not) { return; }

        // after log file parsing, we could calculate anything
        // ===================================================
        szperevt = size_root_edm/evtmax;
        szperevt /= (1024*1024); // bytes -> MB
        // ===================================================
        tperevt = time_result.real/evtmax; // unit: s
        evtpersec = evtmax/time_result.real; // unit: evt/s

        // std::cout << "CPU UTILIZATION: " << time_result.utilization() << std::endl;
        time_result.cal_utilization();

        // ===================================================
        // Find best unit

        // sec->min->hr->day
        if (1) {
            std::cout << "BEFORE BEST UNIT: "
                      << time_result.real
                      << range.time_real.unit
                      << std::endl;
        }
        range.time_real.unit = best_unit(time_result.real, "s");
        if (1) {
            std::cout << "AFTER BEST UNIT: "
                      << time_result.real
                      << range.time_real.unit
                      << std::endl;
        }

        range.size_per_evt.judge(szperevt);
        range.time_real.judge(time_result.real);
        range.time_per_evt.judge(tperevt);
        range.evt_per_sec.judge(evtpersec);

        range.cpu_utilization.judge(time_result.cpu_utilization);
    }

    void parse_log_mem() {
        std::ifstream ifs(fn_log_mem.Data());
        std::cout<<"ifs = "<<fn_log_mem<<std::endl;
        std::string tmp_line;
        double virt; // unit: KB
        double phys; // unit: KB
        while(ifs.good()) {
            std::getline(ifs, tmp_line);
            std::stringstream ss;
            ss << tmp_line;
            ss >> virt >> phys;
            if (ss.fail()) {
                continue;
            }
            // if ok, save them
            //std::cout<<"virt = "<<virt<<std::endl;
            mem_virt.push_back(virt);
            mem_phys.push_back(phys);
        }

    }

    void parse_log() {
        std::ifstream ifs(fn_log.Data());

        std::string tmp_line;
        while(ifs.good()) {
            std::getline(ifs, tmp_line);

            if (tmp_line.find("Running @") != string::npos) {
                // Running @ jnws036.ihep.ac.cn on Mon Sep 26 20:57:42 2016
                std::stringstream ss;
                ss << tmp_line;
                std::string tmp_;
                std::string node_;
                ss >> tmp_ >> tmp_ >> node_;
                node = node_;
            } else if (tmp_line.find("real") == 0
                    || tmp_line.find("user") == 0
                    || tmp_line.find("sys") == 0
                    ) {
                time_result.parse(tmp_line);
            } else if (tmp_line.find("[ATR]EvtMax")!=string::npos) {
                parse_evtmax(tmp_line);
            }
        }

    }

    void parse_evtmax(const std::string& s) {
        // note, if evtmax is -1, we need to load root file and count the entries.
        // Because evtmax is an attribute in task, if multiple tasks are used,
        // we will find multiple evtmax.
        int _evtmax = -1; 

        TString ss = s;
        TString tok;
        Ssiz_t from = 0;
        while (ss.Tokenize(tok, from, "=")) {
            if (0) { std::cout << "LTDEBUG " << tok << std::endl; }
            if (tok.IsDigit()) {
                _evtmax = tok.Atoi();
            }
        }

        if (_evtmax != -1) {
            evtmax = _evtmax;
        }

    }
};

struct Drawer {

    void collect_jobs(const TString& inputdir) {
        // collect log.*.txt.mem.usage
        TString directory = gSystem->UnixPathName(gSystem->WorkingDirectory());
        if (inputdir != ".") {
            directory = inputdir;
        }
        std::cout << "Looking log file in " << inputdir << std::endl;

        TString ext = ".txt.mem.usage";
        TSystemDirectory dir(directory, directory);
        TList *files = dir.GetListOfFiles();
        if (files) {
            TSystemFile *file;
            TString fname;
            TString fulln;
            TIter next(files);
            while ((file=(TSystemFile*)next())) {
                fname = file->GetName();
                if (!file->IsDirectory() && fname.EndsWith(ext)) {
                    std::cout<<fname.Data()<<std::endl;
                    if (0) { cout << fname.Data() << endl; }
                    OneJob j;
                    fulln = gSystem->PrependPathName(directory, fname);
                    std::cout<<"load mem file = "<<fulln<<std::endl;
                    j.load_log_mem(fulln);
                    if (0) j.show();
                    // only include it when job is complete
                    jobs.push_back(j);
                    if (j.use_or_not) {
                        jobs.push_back(j);
                    }

                    if (0) { break; }
                }
            }
        }
        std::cout<<"done"<<std::endl;
    }

    void plot_mem(const TString& output_label="") {
        TMultiGraph* mg_virt = new TMultiGraph();
        TMultiGraph* mg_phys = new TMultiGraph();
        for (int i = 0; i < jobs.size(); ++i) {

            std::cout<<"jobs"<<std::endl;
            if (jobs[i].mem_virt.size()<=0) {
                continue;
            }

            TGraph* g_virt = new TGraph();
            TGraph* g_phys = new TGraph();
            for (int j = 0; j < jobs[i].mem_virt.size(); ++j) {
                double t = j*5.; // every 5 seconds
                double m_virt = jobs[i].mem_virt[j]; // KB
                double m_phys = jobs[i].mem_phys[j]; // KB

                t /= 3600; // seconds -> hr
                m_virt /= 1024; // KiB (1024-bytes) -> MB
                m_phys /= 1024; // KiB (1024-bytes) -> MB
                g_virt->SetPoint(j, t, m_virt);
                g_phys->SetPoint(j, t, m_phys);
            }
            mg_virt->Add(g_virt);
            mg_phys->Add(g_phys);
        }

        TCanvas* c = 0;
        c = new TCanvas;
        mg_virt->Draw("ALP");
        mg_virt->GetXaxis()->SetTitle("time (hr)");
        mg_virt->GetYaxis()->SetTitle("Virt Mem (MB)");
        c->Update();
        helper_print(c, "mem_virt", output_label, "");
        c = new TCanvas;
        mg_phys->Draw("ALP");
        mg_phys->GetXaxis()->SetTitle("time (hr)");
        mg_phys->GetYaxis()->SetTitle("Phys Mem (MB)");
        c->Update();
        helper_print(c, "mem_phys", output_label, "");
    }

    void plot_time(const TString& output_label="") {
        // NOTE: the range and bins need to be optimized according to jobs.
        //
        // 0->10s per event, initialization is included
        TH1F* hist_time = new TH1F("htime", "time per event", 
                                    range.time_per_evt.nbins, 
                                    range.time_per_evt.min, 
                                    range.time_per_evt.max); // time per event

        // total time
        // 0->1hr
        TH1F* hist_total_time = new TH1F("htotaltime", "total time", 
                                    range.time_real.nbins, 
                                    range.time_real.min, 
                                    range.time_real.max);

        // events per second (through output)
        // 1->10?
        TH1F* hist_evts_per_sec = new TH1F("hevtspersec", "events per second", 
                                      range.evt_per_sec.nbins, 
                                      range.evt_per_sec.min, 
                                      range.evt_per_sec.max);

        // CPU utilization
        TH1F* hist_cpu_usage = new TH1F("hcpuusage", "CPU utilization",
                                   range.cpu_utilization.nbins,
                                   range.cpu_utilization.min,
                                   range.cpu_utilization.max);
        for (int i = 0; i < jobs.size(); ++i) {
            OneJob& ajob = jobs[i];
            if (ajob.evtmax==-1) {
                continue;
            }

            hist_time->Fill(ajob.tperevt);

            hist_total_time->Fill(ajob.time_result.real);

            hist_evts_per_sec->Fill(ajob.evtpersec);

            hist_cpu_usage->Fill(ajob.time_result.cpu_utilization);
        }
        TCanvas* c = 0;
        c = new TCanvas;
        hist_time->GetXaxis()->SetTitle("time per event (s)");
        hist_time->Draw();
        helper_print(c, "time_per_evt", output_label, "");

        c = new TCanvas;
        hist_total_time->GetXaxis()->SetTitle(Form("time (%s)", range.time_real.unit.Data()));
        hist_total_time->Draw();
        helper_print(c, "time_total", output_label, "");

        c = new TCanvas;
        hist_cpu_usage->GetXaxis()->SetTitle("CPU%");
        hist_cpu_usage->Draw();
        helper_print(c, "cpu_usage", output_label, "");

        c = new TCanvas;
        hist_evts_per_sec->Draw();
        hist_evts_per_sec->GetXaxis()->SetTitle("throughput (events/s)");
        helper_print(c, "evts_per_sec", output_label, "");
    }

    void plot_filesize(const TString& output_label="") {
        // size per event, (MB? 1MB=1024kB)
        TH1F* hist_size = new TH1F("hsize", "size per event", 
                                    range.size_per_evt.nbins, 
                                    range.size_per_evt.min, 
                                    range.size_per_evt.max); // time per event
        for (int i = 0; i < jobs.size(); ++i) {
            OneJob& ajob = jobs[i];
            if (ajob.evtmax==-1) {
                continue;
            }

            hist_size->Fill(ajob.szperevt);
        }
        TCanvas* c = 0;
        c = new TCanvas;
        hist_size->GetXaxis()->SetTitle("size (MB)");
        hist_size->Draw();
        helper_print(c, "size_per_evt", output_label, "");

    };

    std::vector<OneJob> jobs;
};

/*
 * Draw the memeory usage of jobs.
 */
void drawmem(const TString& inputdir=".", const TString& output_label="") {
    Drawer* gDrawer = new Drawer();
    gDrawer->collect_jobs(inputdir);
    std::cout<<"mem:"<<std::endl;
    gDrawer->plot_mem(output_label);
    std::cout<<"time::"<<std::endl;
    //gDrawer->plot_time(output_label);
    //std::cout<<"filesize::"<<std::endl;
    //gDrawer->plot_filesize(output_label);
}
