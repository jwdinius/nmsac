import numpy as np
import argparse 
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import
import json 
import pyNMSAC as nmsac

class Store_as_array(argparse._StoreAction):
    def __call__(self, parser, namespace, values, option_string=None):
        values = np.array(values)
        return super().__call__(parser, namespace, values, option_string)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", help="Input filename")
    parser.add_argument("-r", "--rate", help="Overlap rate", type=float, default=100.0)
    parser.add_argument("-n", "--noise", help="stddev", type=float, default=0.0)
    parser.add_argument("-a", '--angles', action=Store_as_array, help="rotation angles, rpy", type=float, nargs=3)
    parser.add_argument("-t", '--trans', action=Store_as_array, help="translation offset, xyz", type=float, nargs=3)
    parser.add_argument("-p", "--checkplot", action="store_true",  help="Create plot to check transforms")
    parser.add_argument("-s", "--nmsac", action="store_true",  help="run nmsac algorithm")
    parser.add_argument("-o", "--output", help="Output filename (to hold the json)")
    args = parser.parse_args()
    
    if args.input:
        # a sample file that can be used can be pulled from github via:
        # $ wget https://github.com/hagianga21/KittiSynDataGeneration/raw/master/000000.bin
        try:
            scan = np.fromfile(args.input, dtype=np.float32)
        except IOError:
            raise "Couldn't open input file"
        scan = scan.reshape((-1, 4))
        points = scan[:, 0:3]
    else:
        # sample points from the exterior of a cube: [-1, 1]^3
        nPtsPerFace = 100
        nFaces = 6
        np.random.seed(11011)  # XXX needed for repeatability
        sides = 2 * np.random.random((nFaces * nPtsPerFace, 4)) - 1
        sides[:, 2] = 0.  # overwrite z component with 0 - we'll transform the faces later
        sides[:, 3] = 1.  # homogenous component
        sides = sides.T
        # cube is centered at (0, 0)
        H = {
              "neg-z" : np.array([[1.0, 0.0, 0.0, 0.0], [0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 1.0, -1.0], [0.0, 0.0, 0.0, 1.0]]),
              "pos-z" : np.array([[1.0, 0.0, 0.0, 0.0], [0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 1.0, 1.0], [0.0, 0.0, 0.0, 1.0]]),
              "neg-x" : np.array([[0.0, 0.0, 1.0, -1.0], [0.0, 1.0, 0.0, 0.0], [-1.0, 0.0, 0.0, 0.0], [0.0, 0.0, 0.0, 1.0]]),
              "pos-x" : np.array([[0.0, 0.0, 1.0, 1.0], [0.0, 1.0, 0.0, 0.0], [-1.0, 0.0, 0.0, 0.0], [0.0, 0.0, 0.0, 1.0]]),
              "neg-y" : np.array([[1.0, 0.0, 0.0, 0.0], [0.0, 0.0, -1.0, -1.0], [0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 0.0, 1.0]]),
              "pos-y" : np.array([[1.0, 0.0, 0.0, 0.0], [0.0, 0.0, -1.0, 1.0], [0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 0.0, 1.0]])
        }
        cube = np.zeros_like(sides)
        cube[:, 0:nPtsPerFace] = H["neg-z"].dot(sides[:, 0:nPtsPerFace:1])
        cube[:, nPtsPerFace:2*nPtsPerFace:1] = H["pos-z"].dot(sides[:, nPtsPerFace:2*nPtsPerFace:1])
        cube[:, 2*nPtsPerFace:3*nPtsPerFace:1] = H["neg-x"].dot(sides[:, 2*nPtsPerFace:3*nPtsPerFace:1])
        cube[:, 3*nPtsPerFace:4*nPtsPerFace:1] = H["pos-x"].dot(sides[:, 3*nPtsPerFace:4*nPtsPerFace:1])
        cube[:, 4*nPtsPerFace:5*nPtsPerFace:1] = H["neg-y"].dot(sides[:, 4*nPtsPerFace:5*nPtsPerFace:1])
        cube[:, 5*nPtsPerFace:] = H["pos-y"].dot(sides[:, 5*nPtsPerFace:])
        points = cube[0:3, :].T

    if not args.angles:
        rx = -2.5;  ry = 2.8;  rz = -1.5;
    else:
        rx, ry, rz = args.angles

    if not args.trans:
        Tx = -1; Ty = -2; Tz = 3;
    else:
        Tx, Ty, Tz = args.trans

    
    #Overlapping Rate: 10% -> 100%
    OverlappingRate = args.rate
    #Set up transformation
    t = np.array([[Tx], [Ty], [Tz]])

    Rx = np.array([[1, 0, 0], [0, np.cos(rx), -np.sin(rx)], [0, np.sin(rx), np.cos(rx)]])
    Ry = np.array([[np.cos(ry), 0, np.sin(ry)], [0, 1, 0], [-np.sin(ry), 0, np.cos(ry)]])
    Rz = np.array([[np.cos(rz), -np.sin(rz), 0], [np.sin(rz), np.cos(rz), 0], [0, 0, 1]])
    
    R = Rx.dot(Ry).dot(Rz)

    pointsAfter = R.dot(points.T) + t + args.noise*np.random.randn(3, 1)
    pointsAfter = pointsAfter.T
    
    #remove some points when changing overlapping rate
    numRow = pointsAfter.shape[0]
    numRowDeleted = np.around(numRow*(100 - OverlappingRate)/100.0).astype(int)
    
    idx = np.arange(numRowDeleted)
    pointsAfter = np.delete(pointsAfter, idx, axis = 0)

    if args.checkplot:
        # create transform pointsAfter back for check plot only
        pointsAfter_xform = np.linalg.inv(R).dot(pointsAfter.T - np.tile(t, (1, pointsAfter.shape[0])))
        pointsAfter_xform = pointsAfter_xform.T
        
        fig = plt.figure()
        ax = fig.add_subplot(121, projection='3d')
        ax.scatter(points[:, 0], points[:, 1], points[:, 2], 'b.')
        ax.scatter(pointsAfter[:, 0], pointsAfter[:, 1], pointsAfter[:, 2], 'r.')

        # check #
        axx = fig.add_subplot(122, projection='3d')
        axx.scatter(points[:, 0], points[:, 1], points[:, 2], 'b.')
        axx.scatter(pointsAfter_xform[:, 0], pointsAfter_xform[:, 1], pointsAfter_xform[:, 2], 'r.')
        plt.show()

    if args.nmsac:
        pc = nmsac.Config()
        ''' Update the values below, if you wish
        pc.randomSeed = 11011
        pc.printStatus = True
        pc.ps = 0.99
        pc.maxIter = 10000
        pc.minIter = 5
        pc.k = 4
        pc.pointsPerSample = 12
        pc.epsilon = 0.015
        pc.nPairThresh = 5
        pc.pairDistThresh = 0.01
        pc.maxIterIcp = 100
        pc.tolIcp = 1e-8
        pc.outlierRejRatioIcp = 0.2
        '''

        source_pts = points.T
        source_pts = np.vstack((source_pts, np.ones((1, source_pts.shape[1]))))
        target_pts = pointsAfter.T
        target_pts = np.vstack((target_pts, np.ones((1, target_pts.shape[1]))))

        ### the 2 lines below are VERY important, don't remove ###
        source = np.copy(source_pts[:3, :])
        target = np.copy(target_pts[:3, :])
        ##########################################################
        # make the call
        status, Rn, tn, numInliers, numIterations, callTime = nmsac.execute(source, target, pc)
        if not status: 
            print("NMSAC call failed")
        # grab output
        H_out = np.zeros((4, 4))
        H_out[:3, :3] = np.array(Rn)
        H_out[:3, 3] = np.array(tn.reshape((3,)))
        H_out[3, 3] = 1.0
        print("NMSAC took {}msec".format(callTime))
        source_pts_xform = np.dot(H_out, source_pts)

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')
        ax.scatter(source_pts[0, :], source_pts[1, :], source_pts[2, :], 'b.', label='src')
        ax.scatter(target_pts[0, :], target_pts[1, :], target_pts[2, :], 'r.', label='tgt')
        ax.set_title('Two Unaligned Point Clouds')
        ax.set_xlabel('x')
        ax.set_ylabel('y')
        ax.set_zlabel('z')
        ax.legend()
        # check #
        fig1 = plt.figure()
        axx = fig1.add_subplot(111, projection='3d')
        axx.scatter(target_pts[0, :], target_pts[1, :], target_pts[2, :], 'r.', label='tgt')
        axx.scatter(source_pts_xform[0, :], source_pts_xform[1, :], source_pts_xform[2, :], 'b+', label='src xform')
        axx.set_title('Two Aligned Point Clouds - Solution from NMSAC')
        axx.set_xlabel('x')
        axx.set_ylabel('y')
        axx.set_zlabel('z')
        axx.legend(loc='lower left')
        plt.show()

    # write json
    if args.output:
        data = {}
        data["source_pts"] = (points.T).tolist()
        data["target_pts"] = (pointsAfter.T).tolist()
        data["R_true"] = R.tolist()
        data["t_true"] = t.tolist()
        with open(args.output, "w") as f:
            json.dump(data, f)

