#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h> // For PATH_MAX

/* ---------------- Logging ---------------- */
static void log_message(const char *fmt, ...) {
    char msgbuf[1024];
    char eips_command[sizeof("eips \"") + sizeof(msgbuf) + sizeof("\"")];
    char timestr[64];
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", &tm_now);

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
    va_end(ap);

    printf("[%s] %s\n", timestr, msgbuf);
    fflush(stdout);

    FILE *logf = fopen("install.log", "a");
    if (logf) {
        fprintf(logf, "[%s] %s\n", timestr, msgbuf);
        fclose(logf);
    }
    
    /* Extra spaces to cover over existing text */
    snprintf(eips_command, sizeof(eips_command), "eips \"%s                                                                                                                                                                                                                                    \"", msgbuf);
    system(eips_command);
}

/* ---------------- Shell command helpers ---------------- */
static int run_command(const char *cmd) {
    log_message("Running command: %s", cmd);
    int rc = system(cmd);
    if (rc != 0) log_message("Command failed (status %d): %s", rc, cmd);
    return rc;
}

/* ---------------- Binary patching ---------------- */
static int apply_patch(const char *filepath, const unsigned char *find, const unsigned char *replace, size_t len) {
    FILE *f = fopen(filepath, "rb+");
    if (!f) { log_message("Cannot open %s: %s", filepath, strerror(errno)); return -1; }

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -1; }
    long size = ftell(f);
    rewind(f);
    unsigned char *buf = malloc(size);
    if (buf == NULL) {
        log_message("Failed to allocate memory for %s: %s", filepath, strerror(errno));
        fclose(f);
        return -1;
    }
    
    if (fread(buf, 1, size, f) != size) {
        log_message("Failed to read the full contents of %s", filepath);
        free(buf);
        fclose(f);
        return -1;
    }

    int count=0; long pos=-1;
    for (long i=0; i<=size-(long)len; i++)
        if (memcmp(buf+i, find, len)==0) { count++; pos=i; }

    if (count != 1) { log_message("Pattern occurs %d times in %s", count, filepath); free(buf); fclose(f); return -1; }

    if (fseek(f, pos, SEEK_SET) != 0) {
        log_message("Failed to seek in %s: %s", filepath, strerror(errno));
        free(buf);
        fclose(f);
        return -1;
    }
    if (fwrite(replace, 1, len, f) != len) {
        log_message("Failed to write patch to %s: %s", filepath, strerror(errno));
        free(buf);
        fclose(f);
        return -1;
    }
    fclose(f);
    free(buf);
    log_message("Patch applied to %s at offset %ld", filepath,pos);
    return 0;
}

/* ---------------- Path discovery helper ---------------- */
/* Try a list of candidate relative paths under the patched_bin root and
   return the first one that exists. out must be PATH_MAX sized. */
static int locate_candidate(const char *root, const char *candidates[], size_t n, char *out, size_t outlen) {
    for (size_t i = 0; i < n; ++i) {
        if (snprintf(out, outlen, "%s/%s", root, candidates[i]) >= (int)outlen) {
            /* path would be truncated; skip */
            continue;
        }
        if (access(out, F_OK) == 0) {
            /* found */
            return 0;
        }
    }
    /* not found */
    return -1;
}


/* ---------------- Installation ---------------- */
static int do_install(const char *searchengine) {
    log_message("Starting installation");

    /* Clean old logs */
    run_command("rm -rf /mnt/us/extensions/kindle_browser_patch/install.log");

    /* Ensure patched_bin exists cleanly */
    run_command("rm -rf /mnt/us/extensions/kindle_browser_patch/patched_bin");
    
    if (mkdir("/mnt/us/extensions/kindle_browser_patch/patched_bin", 0755) != 0 && errno != EEXIST) {
        log_message("Failed to create directory: %s", strerror(errno));
        return -1;
    }

    /* Copy the original binaries */
    if (run_command("cp /usr/bin/browser /mnt/us/extensions/kindle_browser_patch/patched_bin/") != 0) {
        return -1;
    }
    
    if (run_command("cp -r /usr/bin/chromium /mnt/us/extensions/kindle_browser_patch/patched_bin/") != 0) {
        return -1;
    }
    
    log_message("Locating kindle_browser and libchromium.so...");

    const char *chromium_root = "/mnt/us/extensions/kindle_browser_patch/patched_bin/chromium";
    char kb_path[PATH_MAX];
    char libchromium_path[PATH_MAX];

    /* Candidate locations relative to chromium_root */
    const char *kb_candidates[] = {
        "bin/kindle_browser",
        "kindle_browser"
    };
    const char *lib_candidates[] = {
        "bin/libchromium.so",
        "libchromium.so"
    };

    if (locate_candidate(chromium_root, kb_candidates, sizeof(kb_candidates)/sizeof(kb_candidates[0]),
                         kb_path, sizeof(kb_path)) != 0) {
        log_message("Could not find kindle_browser in expected locations under %s", chromium_root);
        return -1;
    }
    log_message("Found kindle_browser at %s", kb_path);

    if (locate_candidate(chromium_root, lib_candidates, sizeof(lib_candidates)/sizeof(lib_candidates[0]),
                         libchromium_path, sizeof(libchromium_path)) != 0) {
        log_message("Could not find libchromium.so in expected locations under %s", chromium_root);
        return -1;
    }
    log_message("Found libchromium.so at %s", libchromium_path);

    /* Apply the first patch to kindle_browser (handles different models/firmwares) */
    {
        log_message("Patching kindle_browser...");
        
        int patch_applied = 0;

        /* Variant A */
        unsigned char findA[]    = { 0x0c, 0x36, 0x0c, 0x35, 0x00, 0x28, 0xe8, 0xd0, 0x01, 0x25, 0x00, 0xE0, 0x00, 0x25 };
        unsigned char replaceA[] = { 0x0c, 0x36, 0x0c, 0x35, 0x00, 0x28, 0xe8, 0xd0, 0x01, 0x25, 0x00, 0xE0, 0x01, 0x25 };
        if (sizeof(findA) != sizeof(replaceA)) {
            log_message("kindle_browser patch A error: patch length does not match original bytes.");
            return -1;
        }
        if (apply_patch(kb_path, findA, replaceA, sizeof(findA)) == 0) {
            patch_applied = 1;
            log_message("Applied patch variant A to kindle_browser");
        }

        /* Variant B (fallback for another firmware) */
        if (!patch_applied) {
            unsigned char findB[]    = { 0x0c, 0x36, 0x0c, 0x34, 0x00, 0x28, 0xea, 0xd0, 0x01, 0x24, 0x00, 0xe0, 0x00, 0x24 };
            unsigned char replaceB[] = { 0x0c, 0x36, 0x0c, 0x34, 0x00, 0x28, 0xea, 0xd0, 0x01, 0x24, 0x00, 0xe0, 0x01, 0x24 };
            if (sizeof(findB) != sizeof(replaceB)) {
                log_message("kindle_browser patch B error: patch length does not match original bytes.");
                return -1;
            }
            if (apply_patch(kb_path, findB, replaceB, sizeof(findB)) == 0) {
                patch_applied = 1;
                log_message("Applied patch variant B to kindle_browser");
            }
        }

        if (!patch_applied) {
            log_message("Failed to apply any patch variant to kindle_browser, aborting install");
            return -1;
        }
    }

    /* Patch libchromium.so */
    {
        log_message("Patching libchromium.so...");
        
        unsigned char find[] = { 0x02, 0x46, 0x20, 0x46, 0x29, 0x46, 0xff, 0xf7, 0xda, 0xff, 0x08, 0xb1 };
        unsigned char replace[] = { 0x02, 0x46, 0x20, 0x46, 0x29, 0x46, 0xff, 0xf7, 0xda, 0xff, 0x00, 0xbf };
        if (sizeof(find) != sizeof(replace)) {
            log_message("libchromium.so patch error: patch length does not match original bytes.");
            return -1;
        }
        if(apply_patch(libchromium_path, find, replace, sizeof(find)) != 0) {
            log_message("Failed to apply patch to libchromium.so, aborting install");
            return -1;
        }
    }

    /* Optionally patch kindle_browser to replace Google with other search engine */
    if (searchengine) {
        log_message("Patching search engine...");
        const char *find_str = "https://www.google.com/search?q=";
        const char *replace_str = NULL;
        /* Add extra slashes to https:// to pad length, making the string an equal length to the original */
        if (strcmp(searchengine,"duckduckgo")==0) replace_str="https:////www.duckduckgo.com/?q=";
        else if (strcmp(searchengine,"frogfind")==0) replace_str="http:///////www.frogfind.com/?q=";
        else if (strcmp(searchengine,"bing")==0) replace_str="https:////www.bing.com/search?q=";
        if (replace_str) {
            if (strlen(replace_str) != strlen(find_str)) {
                log_message("Search engine patch error: replacement string length does not match original.");
                return -1;
            }
            if (apply_patch(kb_path,
                        (unsigned char*)find_str,
                        (unsigned char*)replace_str,
                        strlen("https://www.google.com/search?q=")) != 0) {
                log_message("Failed to patch search engine, aborting install");
                return -1;
            }
        }
    }

    /* Update the /browser script to exec the patched kindle_browser (which doesn't work in a chroot) */
    if (run_command("sed -i 's|chroot /chroot ||g' /mnt/us/extensions/kindle_browser_patch/patched_bin/browser") != 0) {
        return -1;
    }
    
    if (run_command("sed -i 's|/usr/bin/chromium/bin/kindle_browser|"
                    "/mnt/us/extensions/kindle_browser_patch/patched_bin/chromium/bin/kindle_browser|g' /mnt/us/extensions/kindle_browser_patch/patched_bin/browser") != 0) {
        return -1;
    }
    
    if (run_command("sed -i 's|/usr/bin/chromium/kindle_browser|"
                    "/mnt/us/extensions/kindle_browser_patch/patched_bin/chromium/kindle_browser|g' /mnt/us/extensions/kindle_browser_patch/patched_bin/browser") != 0) {
        return -1;
    }

    /* Update appreg.db to use patched browser, guarding against cases where there's an unfamiliar value already present (might happen on new firmware if the browser substantially changes) */
    if (run_command(
    "sqlite3 /var/local/appreg.db \"UPDATE properties "
    "SET value='/mnt/us/extensions/kindle_browser_patch/patched_bin/browser -j' "
    "WHERE handlerId='com.lab126.browser' AND name='command' "
    "AND (value='/usr/bin/browser -j' OR value='/mnt/us/extensions/kindle_browser_patch/patched_bin/browser -j');\""
) != 0) {
    log_message("Unexpected value in appreg.db");
    return -1;
}

    /* Start browser */
    if (run_command("lipc-set-prop com.lab126.appmgrd start app://com.lab126.browser") != 0) {
        return -1;
    }
    
    return 0;
}

/* ---------------- Uninstallation ---------------- */
static int do_uninstall(void) {
    log_message("Starting uninstallation");
    
    int retcode = 0;
    
    /* Reset DB value to original, guarding against cases where there's an unfamiliar value already present (might happen on new firmware if the browser substantially changes) */
    if (run_command(
        "sqlite3 /var/local/appreg.db \"UPDATE properties "
        "SET value='/usr/bin/browser -j' "
        "WHERE handlerId='com.lab126.browser' AND name='command' "
        "AND (value='/usr/bin/browser -j' OR value='/mnt/us/extensions/kindle_browser_patch/patched_bin/browser -j');\""
    ) != 0) {
        log_message("Unexpected value in appreg.db");
        retcode = -1;
    }

    /* Remove patched binaries */
    if (run_command("rm -rf /mnt/us/extensions/kindle_browser_patch/patched_bin") != 0) {
        retcode = -1;
    }

    return retcode;
}

/* ---------------- Main ---------------- */
int main(int argc, char *argv[]) {
    if (argc < 2) { fprintf(stderr,"Usage: %s install [searchengine] | uninstall\n",argv[0]); return 1; }
    /* Ensure that our binary is located at the correct path, meaning the extension was installed to the correct directory */
    char actual_path[PATH_MAX];
    ssize_t len;
    
    /* Get the absolute path of the running executable using /proc/self/exe */
    len = readlink("/proc/self/exe", actual_path, sizeof(actual_path) - 1);

    if (len == -1) {
        /* Handle error: Could not read the symbolic link (unlikely on standard Linux) */
        perror("Error reading executable path from /proc/self/exe");
        return -1;
    }

    /* Null-terminate the string read by readlink */
    actual_path[len] = '\0';

    if (strcmp(actual_path, "/mnt/us/extensions/kindle_browser_patch/bin/kindle_browser_patch") != 0) {
        // Path does not match, print error and exit
        log_message("Aborting install: the extension was not installed to the correct path. Please ensure it is located at /mnt/us/extensions/kindle_browser_patch");
        return -1;
    }
    
    if (strcmp(argv[1],"install")==0) {
        const char *searchengine = (argc>=3) ? argv[2] : NULL;
        int retcode = do_install(searchengine);
        
        if (retcode == 0) {
            log_message("Successfully installed.");
            sleep(10);
            system("eips 30 30 \"Successfully installed\"");
        }
        else {
            log_message("Failed to install");
        }
        
        return retcode;
    }
    else if (strcmp(argv[1],"uninstall")==0) {
        int retcode = do_uninstall();
        
        if (retcode == 0) {
            log_message("Successfully uninstalled.");
        }
        else {
            log_message("Failed to uninstall");
        }
        
        return retcode;
    }
    else {
        fprintf(stderr,"Unknown command\n");
        return 1;
    }
}

