import sys
import ambuild.runner as runner

run = runner.Runner()
run.options.add_option('--debug', action='store_const', const='1', dest='debug',
                       help='Enable debugging symbols')
run.options.add_option('--playerslots', action='store_const', const='1', dest='playerslots',
                       help='Build for patching max player slots')
run.Configure(sys.path[0])
