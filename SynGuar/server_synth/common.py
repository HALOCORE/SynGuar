from . import hooks

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

    @staticmethod
    def error(obj):
        return bcolors.FAIL + str(obj) + bcolors.ENDC

    @staticmethod
    def warn(obj):
        return bcolors.WARNING + str(obj) + bcolors.ENDC

    @staticmethod
    def info(obj):
        return bcolors.OKCYAN + str(obj) + bcolors.ENDC
    
    @staticmethod
    def ok(obj):
        return bcolors.OKGREEN + str(obj) + bcolors.ENDC


MY_HOOKS = {
    "Scheduler.process_synth_task": hooks.hook_SCHEDULER_process_synth_task
}

class DBG_FLAGS:
    DEBUG = True
    SYNTH_TASK_CACHE_ONLY = False  # debug flag