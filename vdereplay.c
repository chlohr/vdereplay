/* vdereplay
 * Inject packets from a pcap file to a VDE plug
 *
 * (c) C.Lohr 2025
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>
#include <sys/time.h>
#include <stdbool.h>
#include <libvdeplug.h>
#include <libgen.h>
#include <unistd.h>
#include <signal.h>


void usage(char *prog, int e) {
  fprintf(stderr, "Usage: %s -i <input> [-s <speed>] [-d <delay>] [-l <loop>] [-h] [<VNL>]\n"
          "Inject packets from a pcap file to a VDE plug\n"
          "Options:\n"
          " -i input  Pcap file with Ethernet packets\n"
          " -s speed  Speed factor for interpacket delay (default 1.0)\n"
          " -d delay  Fixed interpacket delay in microseconds\n"
          " -l loop   Number of time to replay the pcap file (default 1; 0 for non-stop)\n"
          " -h        Display this help\n"
          " VNL       Virtual Network Locator (e.g. vde:///tmp/vde.ctl) see vde_plug(1)\n",
           basename(prog));
  exit(e);
}


void parse_opts(int argc, char *argv[], char **input, double *speed, unsigned long *delay, bool *fixed, unsigned long *loop, char **vnl) {
  int opt;
  
  while ((opt = getopt(argc, argv, "i:s:d:l:h")) != -1) {
    switch (opt) {
    case 'i':
      *input = optarg;
      break;
    case 's':
      *speed = strtod(optarg, NULL);
      if ( *speed < 0.0 ) {
        fprintf(stderr, "Can't play with a negative speed\n");
        exit(EXIT_FAILURE);
      }
      break;
    case 'd':
      *delay = strtoul(optarg, NULL, 0);
      *fixed = true;
      break;
    case 'l':
      *loop = strtoul(optarg, NULL, 0);
      break;
    case 'h':
      usage(argv[0], EXIT_SUCCESS);
    default:
      fprintf(stderr, "\n");
      usage(argv[0], EXIT_FAILURE);
    }
  }
  if (argc == (optind+1) )
    *vnl = argv[optind];
  if (argc > (optind+1) ) {
    fprintf(stderr, "%s: excess option '%s'\n\n", argv[0], argv[argc-1]);
    usage(argv[0], EXIT_FAILURE);
  }
  if (!*input) {
    fprintf(stderr, "%s: no pcap file\n\n", argv[0]);
    usage(argv[0], EXIT_FAILURE);
  }
}


pcap_t *get_pcap(char *input) {
  pcap_t *pcap;
  char errbuf[PCAP_ERRBUF_SIZE];

  pcap = pcap_open_offline(input, errbuf);
  if (pcap == NULL) {
    fprintf(stderr, "%s: %s\n", input, errbuf);
    exit(EXIT_FAILURE);
  }
  if (pcap_datalink(pcap) != DLT_EN10MB) {
    fprintf(stderr, "%s: no Ethernet packets\n", input);
    exit(EXIT_FAILURE);
  }
  return pcap;
}


void replay(VDECONN *conn, pcap_t *pcap, double speed, unsigned long delay, bool fixed) {
  struct pcap_pkthdr header;
  const u_char *packet;
  struct timeval last, delta;
  bool first = true;

  while ( (packet = pcap_next(pcap, &header)) != NULL ) {

    if (fixed)
      usleep(delay);

    else {
      if ( first ) {
        first = false;
        timerclear(&delta);
      }
      else
        timersub(&header.ts, &last, &delta);
      last = header.ts;
      usleep( speed * (1000000*delta.tv_sec+delta.tv_usec) );
    }

    vde_send(conn, packet, header.caplen, 0);
  }
}



VDECONN *conn;

void bye() { vde_close(conn); }

void byebye(int s) { exit(EXIT_SUCCESS); }


int main(int argc, char **argv) {
  char *input = NULL;
  double speed = 1.0;
  unsigned long delay;
  bool fixed = false;
  unsigned long loop = 1;
  char *vnl = "";

  signal(SIGINT, byebye);
  signal(SIGTERM, byebye);

  parse_opts(argc, argv, &input, &speed, &delay, &fixed, &loop, &vnl);
  
  conn = vde_open(vnl, "vdereplay", NULL);
  if (conn == NULL) {
    fprintf(stderr, "%s: vde_open error '%s'\n\n", argv[0], vnl);
    usage(argv[0], EXIT_FAILURE);
  }
  atexit(bye);

  for (unsigned long n = 0; (loop == 0) || (n < loop); n++) {
    pcap_t *pcap = get_pcap(input);
    replay(conn, pcap, speed, delay, fixed);
    pcap_close(pcap);
  }

  exit(EXIT_SUCCESS);
}
